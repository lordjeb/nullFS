#include "pch.h"
#include <nullFS/ioctl.h>
#include "GlobalData.h"
#include "flowControl.h"
#include "dispatchRoutines.h"

// ---------------------------------------------------------------------------
// Function implementations
//

_Dispatch_type_(IRP_MJ_DEVICE_CONTROL) _Function_class_(IRP_MJ_DEVICE_CONTROL)
    _Function_class_(DRIVER_DISPATCH) extern "C" NTSTATUS
    NfFsdDeviceControl(_In_ PDEVICE_OBJECT volumeDeviceObject, _Inout_ PIRP irp)
{
    PAGED_CODE();

    NTSTATUS rc{ STATUS_INVALID_DEVICE_REQUEST };
    TRY
    {
        PIO_STACK_LOCATION currentIrpStackLocation = IoGetCurrentIrpStackLocation(irp);

        NfDbgPrint(DPFLTR_DEVICE_CONTROL, "%s: [FileObj=%08p, IoCtl=%08x]\n", __FUNCTION__,
                   currentIrpStackLocation->FileObject,
                   currentIrpStackLocation->Parameters.DeviceIoControl.IoControlCode);

        if (NfDeviceIsFileSystemDeviceObject(volumeDeviceObject))
        {
            NfDbgPrint(DPFLTR_DEVICE_CONTROL, "%s: FileSystemDO\n", __FUNCTION__);

            switch (currentIrpStackLocation->Parameters.DeviceIoControl.IoControlCode)
            {
#if defined(DBG)
            case IOCTL_SHUTDOWN:
                NfDbgPrint(DPFLTR_DEVICE_CONTROL, "%s: IOCTL_SHUTDOWN\n", __FUNCTION__);

                if (FlagOn(globalData.flags, NF_GLOBAL_DATA_FLAGS_FILE_SYSTEM_REGISTERED))
                {
                    ClearFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_FILE_SYSTEM_REGISTERED);
                    IoUnregisterFileSystem(globalData.fileSystemDeviceObject);

                    // Complete hack that will allow our driver to unload. It appears that IopCheckDriverUnload looks
                    // for this undocumented 0x80 flag, and refuses to unload the driver, even after it has done all the
                    // checks for reference counts and attached devices and all that.
                    globalData.fileSystemDeviceObject->DriverObject->Flags &= ~0x80;
                }

                rc = STATUS_SUCCESS;
                break;
#endif

            default:
                NfDbgPrint(DPFLTR_DEVICE_CONTROL, "%s: Unknown DeviceIoControl.IoControlCode\n", __FUNCTION__);
                break;
            }

            LEAVE();
        }

        NfDbgPrint(DPFLTR_DEVICE_CONTROL, "%s: Unrecognized device object\n", __FUNCTION__);
    }
    FINALLY
    {
        return NfCompleteRequest(irp, rc, 0);
    }
}
