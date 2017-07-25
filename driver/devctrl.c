#include "common.h"
#include <ntifs.h>
#include "ioctl.h"

// ---------------------------------------------------------------------------
// Assign text sections for each routine.
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, NfFsdDeviceControl)
#endif

// ---------------------------------------------------------------------------
// Function implementations
//

_Function_class_(IRP_MJ_DEVICE_CONTROL)
_Function_class_(DRIVER_DISPATCH)
NTSTATUS NfFsdDeviceControl(_In_ PDEVICE_OBJECT volumeDeviceObject, _Inout_ PIRP irp)
{
    NTSTATUS rc = STATUS_ILLEGAL_FUNCTION;

    KdPrint(("nullFS: IRP_MJ_DEVICE_CONTROL\n"));

    if (NfDeviceIsFileSystemDeviceObject((PDEVICE_OBJECT)volumeDeviceObject))
    {
        PIO_STACK_LOCATION currentIrpStackLocation = IoGetCurrentIrpStackLocation(irp);

        switch (currentIrpStackLocation->Parameters.DeviceIoControl.IoControlCode)
        {
#ifdef DBG
        case IOCTL_SHUTDOWN:
            KdPrint(("nullFS: IOCTL_SHUTDOWN\n"));
            
            if (FlagOn(globalData.flags, NF_GLOBAL_DATA_FLAGS_FILE_SYSTEM_REGISTERED))
            {
                ClearFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_FILE_SYSTEM_REGISTERED);
                IoUnregisterFileSystem(globalData.fileSystemDeviceObject);

                // Complete hack that will allow our driver to unload. It appears that IopCheckDriverUnload looks for this
                // undocumented 0x80 flag, and refuses to unload the driver, even after it has done all the checks for
                // reference counts and attached devices and all that.
                globalData.fileSystemDeviceObject->DriverObject->Flags &= ~0x80;
            }
            
            rc = STATUS_SUCCESS;
#endif

        default:
            KdPrint(("nullFS: IOCTL_Unknown\n"));
            break;
        }

        FUNCTION_EXIT;
    }

    if (NfDeviceIsDiskDeviceObject((PDEVICE_OBJECT)volumeDeviceObject))
    {
        KdPrint(("nullFS: Volume device object\n"));
        FUNCTION_EXIT;
    }

    KdPrint(("nullFS: Unrecognized device object\n"));

function_exit:

    return NfCompleteRequest(irp, rc, 0);
}
