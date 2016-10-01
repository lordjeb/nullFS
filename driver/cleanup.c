#include "common.h"
#include <ntifs.h>

// ---------------------------------------------------------------------------
// Assign text sections for each routine.
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, NfFsdCleanup)
#endif

// ---------------------------------------------------------------------------
// Function implementations
//

_Function_class_(IRP_MJ_CLEANUP)
_Function_class_(DRIVER_DISPATCH)
NTSTATUS NfFsdCleanup(
    _In_ NfVolumeDeviceObject* volumeDeviceObject,
    _Inout_ PIRP irp
    )
{
    UNREFERENCED_PARAMETER(volumeDeviceObject);
    UNREFERENCED_PARAMETER(irp);

    KdPrint(("nullFS: NfFsdCleanup\n"));

    return STATUS_SUCCESS;
}
