#include "pch.h"
#include "flowControl.h"
#include "dispatchRoutines.h"

// ---------------------------------------------------------------------------
// Function implementations
//

_Dispatch_type_(IRP_MJ_CREATE) _Function_class_(IRP_MJ_CREATE) _Function_class_(DRIVER_DISPATCH) extern "C" NTSTATUS
    NfFsdCreate(_In_ PDEVICE_OBJECT volumeDeviceObject, _Inout_ PIRP irp)
{
    PAGED_CODE();

    NTSTATUS rc{ STATUS_ILLEGAL_FUNCTION };
    __try
    {
        ULONG_PTR information = 0; // TODO: What to do with this?
        PIO_STACK_LOCATION currentIrpStackLocation = IoGetCurrentIrpStackLocation(irp);

        NfDbgPrint(DPFLTR_CREATE, "IRP_MJ_CREATE [FileObj=%08p]\n", currentIrpStackLocation->FileObject);

        if (NfDeviceIsFileSystemDeviceObject(volumeDeviceObject))
        {
            NfDbgPrint(DPFLTR_CREATE, "IRP_MJ_CREATE: FileSystemDO\n");
            information = FILE_OPENED;
            LEAVE_WITH(rc = STATUS_SUCCESS);
        }

        if (NfDeviceIsDiskDeviceObject(volumeDeviceObject))
        {
            NfDbgPrint(DPFLTR_CREATE, "IRP_MJ_CREATE: DiskDO\n");
            information = FILE_OPENED;
            LEAVE_WITH(rc = STATUS_SUCCESS);
        }

        NfDbgPrint(DPFLTR_CREATE, "IRP_MJ_CREATE: Unrecognized device object\n");
    }
    __finally
    {
        return NfCompleteRequest(irp, rc, 0);
    }
}
