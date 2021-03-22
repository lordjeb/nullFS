#include "pch.h"
#include "debug.h"
#include "support.h"
#include "flowControl.h"
#include "dispatchRoutines.h"

// ---------------------------------------------------------------------------
// Function implementations
//

_Dispatch_type_(IRP_MJ_CLEANUP) _Function_class_(IRP_MJ_CLEANUP) _Function_class_(DRIVER_DISPATCH) extern "C" NTSTATUS
    NfFsdCleanup(_In_ PDEVICE_OBJECT volumeDeviceObject, _Inout_ PIRP irp)
{
    PAGED_CODE();

    NTSTATUS rc{ STATUS_ILLEGAL_FUNCTION };
    __try
    {
        PIO_STACK_LOCATION currentIrpStackLocation = IoGetCurrentIrpStackLocation(irp);

        NfDbgPrint(DPFLTR_CLEANUP, "IRP_MJ_CLEANUP [FileObj=%08p]\n", currentIrpStackLocation->FileObject);

        if (NfDeviceIsFileSystemDeviceObject(volumeDeviceObject))
        {
            NfDbgPrint(DPFLTR_CLEANUP, "IRP_MJ_CLEANUP: FileSystemDO\n");
            LEAVE_WITH(rc = STATUS_SUCCESS);
        }

        NfDbgPrint(DPFLTR_CLEANUP, "IRP_MJ_CLEANUP: Unrecognized device object\n");
    }
    __finally
    {
        return NfCompleteRequest(irp, rc, 0);
    }
}
