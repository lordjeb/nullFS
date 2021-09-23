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

        if (NfDeviceIsFileSystemDeviceObject(volumeDeviceObject))
        {
            switch (currentIrpStackLocation->Parameters.DeviceIoControl.IoControlCode)
            {
#if defined(DBG)
            case IOCTL_SHUTDOWN:
                NfTraceCommon(WINEVENT_LEVEL_VERBOSE, "DeviceControlShutdown", TraceLoggingPointer(volumeDeviceObject));

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
                NfTraceCommon(WINEVENT_LEVEL_VERBOSE, "DeviceControlUnknownControlCode",
                              TraceLoggingPointer(volumeDeviceObject));
                break;
            }

            LEAVE();
        }

        NfTraceCommon(WINEVENT_LEVEL_VERBOSE, "DeviceControlUnknownDeviceObject",
                      TraceLoggingPointer(volumeDeviceObject));
    }
    FINALLY
    {
        return NfCompleteRequest(irp, rc, 0);
    }
}
