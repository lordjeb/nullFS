#include "common.h"
#include <ntifs.h>

// ---------------------------------------------------------------------------
// Assign text sections for each routine.
//

#if defined(ALLOC_PRAGMA)
#pragma alloc_text(PAGE, NfFsdFileSystemControl)
#endif

// ---------------------------------------------------------------------------
// Function implementations
//

_Dispatch_type_(IRP_MJ_FILE_SYSTEM_CONTROL)
_Function_class_(IRP_MJ_FILE_SYSTEM_CONTROL)
_Function_class_(DRIVER_DISPATCH)
NTSTATUS NfFsdFileSystemControl(_In_ PDEVICE_OBJECT volumeDeviceObject, _Inout_ PIRP irp)
{
    NTSTATUS rc = STATUS_ILLEGAL_FUNCTION;
    PIO_STACK_LOCATION currentIrpStackLocation = IoGetCurrentIrpStackLocation(irp);

    PAGED_CODE();

    NfDbgPrint(DPFLTR_FS_CONTROL, "IRP_MJ_FILE_SYSTEM_CONTROL [FileObj=%08p]\n", currentIrpStackLocation->FileObject);

    if (NfDeviceIsFileSystemDeviceObject((PDEVICE_OBJECT)volumeDeviceObject))
    {
        NfDbgPrint(DPFLTR_FS_CONTROL, "IRP_MJ_FILE_SYSTEM_CONTROL: FileSystemDO\n");

        switch (currentIrpStackLocation->MinorFunction)
        {
        case IRP_MN_MOUNT_VOLUME:
            NfDbgPrint(DPFLTR_FS_CONTROL, "IRP_MJ_FILE_SYSTEM_CONTROL: IRP_MN_MOUNT_VOLUME\n");
            break;

        default:
            NfDbgPrint(DPFLTR_FS_CONTROL, "IRP_MJ_FILE_SYSTEM_CONTROL: Unknown MinorFunction\n");
            break;
        }

        FUNCTION_EXIT;
    }

    if (NfDeviceIsDiskDeviceObject((PDEVICE_OBJECT)volumeDeviceObject))
    {
        NfDbgPrint(DPFLTR_FS_CONTROL, "IRP_MJ_FILE_SYSTEM_CONTROL: DiskDO\n");
        FUNCTION_EXIT;
    }

    NfDbgPrint(DPFLTR_FS_CONTROL, "IRP_MJ_FILE_SYSTEM_CONTROL: Unrecognized device object\n");

function_exit:

    return NfCompleteRequest(irp, rc, 0);
}
