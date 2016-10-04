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
NTSTATUS NfFsdDeviceControl(
    _In_ NfVolumeDeviceObject* volumeDeviceObject,
    _Inout_ PIRP irp
    )
{
    NTSTATUS status = STATUS_ILLEGAL_FUNCTION;

    KdPrint(("nullFS: NfFsdDeviceControl\n"));

    if (NfDeviceIsControlDeviceObject((PDEVICE_OBJECT)volumeDeviceObject))
    {
        PIO_STACK_LOCATION currentIrpStackLocation = IoGetCurrentIrpStackLocation(irp);

        switch (currentIrpStackLocation->Parameters.DeviceIoControl.IoControlCode)
        {
#ifdef DBG
        case IOCTL_SHUTDOWN:
            KdPrint(("nullFS: Shutdown device control code\n"));
            
            if (FlagOn(globalData.flags, NF_GLOBAL_DATA_FLAGS_FILE_SYSTEM_REGISTERED))
            {
                ClearFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_FILE_SYSTEM_REGISTERED);
                IoUnregisterFileSystem(globalData.controlDeviceObject);

                // Complete hack that will allow our driver to unload. It appears that IopCheckDriverUnload looks for this
                // undocumented 0x80 flag, and refuses to unload the driver, even after it has done all the checks for
                // reference counts and attached devices and all that.
                globalData.controlDeviceObject->DriverObject->Flags &= ~0x80;
            }
            
            status = irp->IoStatus.Status = STATUS_SUCCESS;
            irp->IoStatus.Information = FILE_OPENED;
            IoCompleteRequest(irp, IO_DISK_INCREMENT);
            break;
#endif

        default:
            KdPrint(("nullFS: Unknown device control code\n"));
            break;
        }
    }

    return status;
}
