#include "common.h"
#include <ntifs.h>

// ---------------------------------------------------------------------------
// Assign text sections for each routine.
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, NfFsdCreate)
#endif

// ---------------------------------------------------------------------------
// Function implementations
//

_Function_class_(IRP_MJ_CREATE)
_Function_class_(DRIVER_DISPATCH)
NTSTATUS NfFsdCreate(
    _In_ NfVolumeDeviceObject* volumeDeviceObject,
    _Inout_ PIRP irp
    )
{
    KdPrint(("nullFS: NfFsdCreate\n"));

    if (NfDeviceIsControlDeviceObject((PDEVICE_OBJECT)volumeDeviceObject))
    {
        KdPrint(("nullFS: Opening Control Device Object\n"));
        irp->IoStatus.Status = STATUS_SUCCESS;
        irp->IoStatus.Information = FILE_OPENED;
        IoCompleteRequest(irp, IO_DISK_INCREMENT);
        return STATUS_SUCCESS;
    }

    return STATUS_SUCCESS;
}
