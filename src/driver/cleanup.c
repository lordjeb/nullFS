#include "pch.h"
#include <ntifs.h>

// ---------------------------------------------------------------------------
// Assign text sections for each routine.
//

#if defined(ALLOC_PRAGMA)
#pragma alloc_text(PAGE, NfFsdCleanup)
#endif

// ---------------------------------------------------------------------------
// Function implementations
//

_Dispatch_type_(IRP_MJ_CLEANUP) _Function_class_(IRP_MJ_CLEANUP) _Function_class_(DRIVER_DISPATCH) NTSTATUS
    NfFsdCleanup(_In_ PDEVICE_OBJECT volumeDeviceObject, _Inout_ PIRP irp)
{
    NTSTATUS rc = STATUS_ILLEGAL_FUNCTION;
    PIO_STACK_LOCATION currentIrpStackLocation = IoGetCurrentIrpStackLocation(irp);

    PAGED_CODE();

    NfDbgPrint(DPFLTR_CLEANUP, "IRP_MJ_CLEANUP [FileObj=%08p]\n", currentIrpStackLocation->FileObject);

    if (NfDeviceIsFileSystemDeviceObject(volumeDeviceObject))
    {
        NfDbgPrint(DPFLTR_CLEANUP, "IRP_MJ_CLEANUP: FileSystemDO\n");
        FUNCTION_EXIT_WITH(rc = STATUS_SUCCESS);
    }

    if (NfDeviceIsDiskDeviceObject(volumeDeviceObject))
    {
        NfDbgPrint(DPFLTR_CLEANUP, "IRP_MJ_CLEANUP: DiskDO\n");
        FUNCTION_EXIT_WITH(rc = STATUS_SUCCESS);
    }

    NfDbgPrint(DPFLTR_CLEANUP, "IRP_MJ_CLEANUP: Unrecognized device object\n");

function_exit:

    return NfCompleteRequest(irp, rc, 0);
}
