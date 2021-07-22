#include "pch.h"
#include "dispatchRoutines.h"

// ---------------------------------------------------------------------------
// Function implementations
//

_Dispatch_type_(IRP_MJ_CLOSE) _Function_class_(IRP_MJ_CLOSE) _Function_class_(DRIVER_DISPATCH) extern "C" NTSTATUS
    NfFsdClose(_In_ PDEVICE_OBJECT deviceObject, _Inout_ PIRP irp)
{
    PAGED_CODE();

    NTSTATUS rc{ STATUS_ILLEGAL_FUNCTION };
    TRY
    {
        PIO_STACK_LOCATION currentIrpStackLocation = IoGetCurrentIrpStackLocation(irp);

        NfDbgPrint(DPFLTR_CLOSE, "%s: [FileObj=%08p]\n", __FUNCTION__, currentIrpStackLocation->FileObject);

        if (NfDeviceIsFileSystemDeviceObject(deviceObject))
        {
            NfDbgPrint(DPFLTR_CREATE, "%s: FileSystemDO\n", __FUNCTION__);
            LEAVE_WITH(rc = STATUS_SUCCESS);
        }

        NfDbgPrint(DPFLTR_CLOSE, "%s: Unrecognized device object [DevObj=%p]\n", __FUNCTION__, deviceObject);
    }
    FINALLY
    {
        return NfCompleteRequest(irp, rc, 0);
    }
}
