#include "pch.h"
#include "flowControl.h"
#include "dispatchRoutines.h"

// ---------------------------------------------------------------------------
// Function implementations
//

_Dispatch_type_(IRP_MJ_FILE_SYSTEM_CONTROL) _Function_class_(IRP_MJ_FILE_SYSTEM_CONTROL)
    _Function_class_(DRIVER_DISPATCH) extern "C" NTSTATUS
    NfFsdFileSystemControl(_In_ PDEVICE_OBJECT volumeDeviceObject, _Inout_ PIRP irp)
{
    PAGED_CODE();

    NTSTATUS rc{ STATUS_ILLEGAL_FUNCTION };
    __try
    {
        PIO_STACK_LOCATION currentIrpStackLocation = IoGetCurrentIrpStackLocation(irp);

        NfDbgPrint(DPFLTR_FS_CONTROL, "IRP_MJ_FILE_SYSTEM_CONTROL [FileObj=%08p]\n",
                   currentIrpStackLocation->FileObject);

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

            LEAVE();
        }

        if (NfDeviceIsDiskDeviceObject((PDEVICE_OBJECT)volumeDeviceObject))
        {
            NfDbgPrint(DPFLTR_FS_CONTROL, "IRP_MJ_FILE_SYSTEM_CONTROL: DiskDO\n");
            LEAVE();
        }

        NfDbgPrint(DPFLTR_FS_CONTROL, "IRP_MJ_FILE_SYSTEM_CONTROL: Unrecognized device object\n");
    }
    __finally
    {
        return NfCompleteRequest(irp, rc, 0);
    }
}
