#include <ntifs.h>
#include "flowControl.h"
#include "dispatchRoutines.h"

// ---------------------------------------------------------------------------
// Assign text sections for each routine.
//

#if defined(ALLOC_PRAGMA)
#pragma alloc_text(PAGE, NfFsdCreate)
#endif

// ---------------------------------------------------------------------------
// Function implementations
//

_Dispatch_type_(IRP_MJ_CREATE) _Function_class_(IRP_MJ_CREATE) _Function_class_(DRIVER_DISPATCH) NTSTATUS
    NfFsdCreate(_In_ PDEVICE_OBJECT volumeDeviceObject, _Inout_ PIRP irp)
{
    NTSTATUS rc = STATUS_ILLEGAL_FUNCTION;
    ULONG_PTR information = 0;
    PIO_STACK_LOCATION currentIrpStackLocation = IoGetCurrentIrpStackLocation(irp);

    PAGED_CODE();

    NfDbgPrint(DPFLTR_CREATE, "IRP_MJ_CREATE [FileObj=%08p]\n", currentIrpStackLocation->FileObject);

    if (NfDeviceIsFileSystemDeviceObject(volumeDeviceObject))
    {
        NfDbgPrint(DPFLTR_CREATE, "IRP_MJ_CREATE: FileSystemDO\n");
        rc = STATUS_SUCCESS;
        information = FILE_OPENED;
        FUNCTION_EXIT;
    }

    if (NfDeviceIsDiskDeviceObject(volumeDeviceObject))
    {
        NfDbgPrint(DPFLTR_CREATE, "IRP_MJ_CREATE: DiskDO\n");
        rc = STATUS_SUCCESS;
        information = FILE_OPENED;
        FUNCTION_EXIT;
    }

    NfDbgPrint(DPFLTR_CREATE, "IRP_MJ_CREATE: Unrecognized device object\n");

function_exit:

    return NfCompleteRequest(irp, rc, information);
}
