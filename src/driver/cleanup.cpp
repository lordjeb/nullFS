#include "pch.h"
#include "support.h"
#include "flowControl.h"
#include "dispatchRoutines.h"

// ---------------------------------------------------------------------------
// Function implementations
//

_Dispatch_type_(IRP_MJ_CLEANUP) _Function_class_(IRP_MJ_CLEANUP) _Function_class_(DRIVER_DISPATCH) extern "C" NTSTATUS
    NfFsdCleanup(_In_ PDEVICE_OBJECT deviceObject, _Inout_ PIRP irp)
{
    PAGED_CODE();

    NTSTATUS rc{ STATUS_ILLEGAL_FUNCTION };
    TRY
    {
        if (NfDeviceIsFileSystemDeviceObject(deviceObject))
        {
            NfTraceCleanup(WINEVENT_LEVEL_VERBOSE, "CleanupFsdo", TraceLoggingPointer(deviceObject));
            LEAVE_WITH(rc = STATUS_SUCCESS);
        }
    }
    FINALLY
    {
        return NfCompleteRequest(irp, rc, 0);
    }
}
