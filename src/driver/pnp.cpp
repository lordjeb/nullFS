#include "pch.h"
#include "flowControl.h"
#include "dispatchRoutines.h"
#include "nodeType.h"
#include "struct.h"

NTSTATUS
NfPnpCompletionRoutine(_In_ PDEVICE_OBJECT deviceObject, _In_ PIRP irp,
                       _In_reads_opt_(_Inexpressible_("varies")) PVOID contxt)
{
    UNREFERENCED_PARAMETER(deviceObject);
    UNREFERENCED_PARAMETER(irp);

    const auto event = static_cast<PKEVENT>(contxt);
    KeSetEvent(event, 0, FALSE);
    return STATUS_MORE_PROCESSING_REQUIRED;
}

_Dispatch_type_(IRP_MJ_PNP) _Function_class_(IRP_MJ_PNP) _Function_class_(DRIVER_DISPATCH) extern "C" NTSTATUS
    NfFsdPnp(_In_ PDEVICE_OBJECT deviceObject, _Inout_ PIRP irp)
{
    NTSTATUS rc{ STATUS_NOT_IMPLEMENTED };
    TRY
    {
        const auto irpSp = IoGetCurrentIrpStackLocation(irp);

        const auto nfDeviceObject = reinterpret_cast<NfVolumeDeviceObject*>(irpSp->DeviceObject);

        // TODO: Enable the node type check below for safety and make sure it works without PNP crash due to completing Irp with non-success status
#pragma warning(suppress : 28175)   // Ok for file system driver
        if (nfDeviceObject->deviceObject.Size != sizeof(NfVolumeDeviceObject) /*||
            NodeType(nfDeviceObject) != NfNodeTypeCodeVolumeControlBlock*/)
        {
            LEAVE_WITH(rc = STATUS_INVALID_PARAMETER);
        }

        const auto vcb = &(nfDeviceObject->vcb);

        switch (irpSp->MinorFunction)
        {
        case IRP_MN_SURPRISE_REMOVAL: {
            NfTracePnp(WINEVENT_LEVEL_INFO, "Pnp_SurpriseRemoval", TraceLoggingPointer(deviceObject));

            // TODO: Tear down the Vdo created in NfMountVolume
            // TODO: Check reference counts on vpb, open count, etc. (See FatCheckForDismount)
            //
            // Forward this down to the storage...

            KEVENT event;
            IoCopyCurrentIrpStackLocationToNext(irp);
            KeInitializeEvent(&event, NotificationEvent, FALSE);
            IoSetCompletionRoutine(irp, NfPnpCompletionRoutine, &event, TRUE, TRUE, TRUE);

            rc = IoCallDriver(vcb->targetDeviceObject, irp);
            if (rc == STATUS_PENDING)
            {
                KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, nullptr);
                rc = irp->IoStatus.Status;
            }

            WI_ClearFlag(vcb->vpb->Flags, VPB_MOUNTED);

            NfUninitializeVcb(vcb);

            const auto volumeDeviceObject = vcb->vpb->DeviceObject;
            vcb->vpb->DeviceObject = nullptr;

            IoDeleteDevice(volumeDeviceObject);
            break;
        }

        default:
            NfTracePnp(WINEVENT_LEVEL_INFO, "Pnp_UnhandledMinorFn", TraceLoggingPointer(deviceObject),
                       TraceLoggingUInt8(irpSp->MinorFunction, "MinorFunction"));
            break;
        }
    }
    FINALLY
    {
        return NfCompleteRequest(irp, rc, 0);
    }
}
