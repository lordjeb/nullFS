#include "pch.h"
#include "dispatchRoutines.h"

// ---------------------------------------------------------------------------
// Function implementations
//

_Dispatch_type_(IRP_MJ_CLOSE) _Function_class_(IRP_MJ_CLOSE) _Function_class_(DRIVER_DISPATCH) extern "C" NTSTATUS
    NfFsdClose(_In_ PDEVICE_OBJECT volumeDeviceObject, _Inout_ PIRP irp)
{
    PAGED_CODE();

    NTSTATUS rc{ STATUS_ILLEGAL_FUNCTION };
    __try
    {
        PIO_STACK_LOCATION currentIrpStackLocation = IoGetCurrentIrpStackLocation(irp);

        NfDbgPrint(DPFLTR_CLOSE, "IRP_MJ_CLOSE [FileObj=%08p]\n", currentIrpStackLocation->FileObject);

        if (NfDeviceIsFileSystemDeviceObject(volumeDeviceObject))
        {
            NfDbgPrint(DPFLTR_CREATE, "IRP_MJ_CLOSE: FileSystemDO\n");
            LEAVE_WITH(rc = STATUS_SUCCESS);
        }

        if (NfDeviceIsDiskDeviceObject(volumeDeviceObject))
        {
            NfDbgPrint(DPFLTR_CREATE, "IRP_MJ_CLOSE: DiskDO\n");
            LEAVE_WITH(rc = STATUS_SUCCESS);
        }

        NfDbgPrint(DPFLTR_CLOSE, "Unrecognized device object\n");
    }
    __finally
    {
        return NfCompleteRequest(irp, rc, 0);
    }
}
