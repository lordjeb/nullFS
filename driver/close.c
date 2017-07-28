#include "common.h"
#include <ntifs.h>

// ---------------------------------------------------------------------------
// Assign text sections for each routine.
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, NfFsdClose)
#endif

// ---------------------------------------------------------------------------
// Function implementations
//

_Function_class_(IRP_MJ_CLOSE)
_Function_class_(DRIVER_DISPATCH)
NTSTATUS NfFsdClose(_In_ PDEVICE_OBJECT volumeDeviceObject, _Inout_ PIRP irp)
{
    NTSTATUS rc = STATUS_ILLEGAL_FUNCTION;

    NfDbgPrint(DPFLTR_CLOSE, "nullFS: IRP_MJ_CLOSE\n");

    if (NfDeviceIsFileSystemDeviceObject((PDEVICE_OBJECT)volumeDeviceObject) ||
        NfDeviceIsDiskDeviceObject((PDEVICE_OBJECT)volumeDeviceObject))
    {
        FUNCTION_EXIT_WITH(rc = STATUS_SUCCESS)
    }

    NfDbgPrint(DPFLTR_CLOSE, "nullFS: Unrecognized device object\n");

function_exit:

    return NfCompleteRequest(irp, rc, 0);
}
