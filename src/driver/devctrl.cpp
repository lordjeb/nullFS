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
    NfFsdDeviceControl(_In_ PDEVICE_OBJECT deviceObject, _Inout_ PIRP irp)
{
    NTSTATUS rc{ STATUS_NOT_IMPLEMENTED };
    TRY
    {
        auto irpSp = IoGetCurrentIrpStackLocation(irp);

        switch (irpSp->Parameters.DeviceIoControl.IoControlCode)
        {
#if defined(DBG)
        case IOCTL_NULLFS_SHUTDOWN:
            NfTraceDeviceControl(WINEVENT_LEVEL_VERBOSE, "Shutdown", TraceLoggingPointer(deviceObject));

            if (FlagOn(globalData.flags, NF_GLOBAL_DATA_FLAGS_FILE_SYSTEM_REGISTERED))
            {
                ClearFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_FILE_SYSTEM_REGISTERED);
                IoUnregisterFileSystem(globalData.fileSystemDeviceObject);

                // Complete hack that will allow our driver to unload. It appears that IopCheckDriverUnload looks
                // for this undocumented 0x80 flag, and refuses to unload the driver, even after it has done all the
                // checks for reference counts and attached devices and all that.
#pragma warning(suppress : 28175)   // Ok for file system driver
#pragma warning(suppress : 28176)   // Ok for file system driver
                globalData.fileSystemDeviceObject->DriverObject->Flags &= ~0x80;
            }

            rc = STATUS_SUCCESS;
            break;
#endif

        default:
            NfTraceDeviceControl(WINEVENT_LEVEL_VERBOSE, "UnhandledIoControlCode", TraceLoggingPointer(deviceObject),
                                 TraceLoggingULong(irpSp->Parameters.DeviceIoControl.IoControlCode, "IoControlCode"));
            break;
        }
    }
    FINALLY
    {
        return NfCompleteRequest(irp, rc, 0);
    }
}
