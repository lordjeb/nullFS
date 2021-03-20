#include <ntifs.h>
#include <nullFS/ioctl.h>
#include "flowControl.h"
#include "dispatchRoutines.h"

// ---------------------------------------------------------------------------
// Function implementations
//

_Dispatch_type_(IRP_MJ_DEVICE_CONTROL) _Function_class_(IRP_MJ_DEVICE_CONTROL)
    _Function_class_(DRIVER_DISPATCH) extern "C" NTSTATUS
    NfFsdDeviceControl(_In_ PDEVICE_OBJECT volumeDeviceObject, _Inout_ PIRP irp)
{
    NTSTATUS rc = STATUS_ILLEGAL_FUNCTION;
    PIO_STACK_LOCATION currentIrpStackLocation = IoGetCurrentIrpStackLocation(irp);

    PAGED_CODE();

    NfDbgPrint(DPFLTR_DEVICE_CONTROL, "IRP_MJ_DEVICE_CONTROL [FileObj=%08p]\n", currentIrpStackLocation->FileObject);

    if (NfDeviceIsFileSystemDeviceObject(volumeDeviceObject))
    {
        NfDbgPrint(DPFLTR_DEVICE_CONTROL, "IRP_MJ_DEVICE_CONTROL: FileSystemDO\n");

        switch (currentIrpStackLocation->Parameters.DeviceIoControl.IoControlCode)
        {
#if defined(DBG)
        case IOCTL_SHUTDOWN:
            NfDbgPrint(DPFLTR_DEVICE_CONTROL, "IRP_MJ_DEVICE_CONTROL: IOCTL_SHUTDOWN\n");

            if (FlagOn(globalData.flags, NF_GLOBAL_DATA_FLAGS_FILE_SYSTEM_REGISTERED))
            {
                ClearFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_FILE_SYSTEM_REGISTERED);
                IoUnregisterFileSystem(globalData.fileSystemDeviceObject);

                // Complete hack that will allow our driver to unload. It appears that IopCheckDriverUnload looks for
                // this undocumented 0x80 flag, and refuses to unload the driver, even after it has done all the checks
                // for reference counts and attached devices and all that.
#pragma warning(suppress : 28175)
#pragma warning(suppress : 28176)
                globalData.fileSystemDeviceObject->DriverObject->Flags &= ~0x80;
            }

            rc = STATUS_SUCCESS;
            break;
#endif

        default:
            NfDbgPrint(DPFLTR_DEVICE_CONTROL, "IRP_MJ_DEVICE_CONTROL: Unknown DeviceIoControl.IoControlCode\n");
            break;
        }

        FUNCTION_EXIT;
    }

    if (NfDeviceIsDiskDeviceObject(volumeDeviceObject))
    {
        NfDbgPrint(DPFLTR_DEVICE_CONTROL, "IRP_MJ_DEVICE_CONTROL: DiskDO\n");
        FUNCTION_EXIT;
    }

    NfDbgPrint(DPFLTR_DEVICE_CONTROL, "IRP_MJ_DEVICE_CONTROL: Unrecognized device object\n");

function_exit:

    return NfCompleteRequest(irp, rc, 0);
}
