#include "pch.h"
#include "dispatchRoutines.h"

// ---------------------------------------------------------------------------
// Function implementations
//

_Dispatch_type_(IRP_MJ_CLOSE) _Function_class_(IRP_MJ_CLOSE) _Function_class_(DRIVER_DISPATCH) extern "C" NTSTATUS
    NfFsdClose(_In_ PDEVICE_OBJECT deviceObject, _Inout_ PIRP irp)
{
    NTSTATUS  rc;
    ULONG_PTR information{ 0 };
    TRY
    {
        const auto irpSp = IoGetCurrentIrpStackLocation(irp);

        // If we were called with our file system device object instead of a volume device object just complete with
        // STATUS_SUCCESS

        if (NfDeviceIsFileSystemDeviceObject(deviceObject))
        {
            NfTraceClose(WINEVENT_LEVEL_VERBOSE, "Close_Fsdo", TraceLoggingPointer(deviceObject));
            information = FILE_OPENED;
            LEAVE_WITH(rc = STATUS_SUCCESS);
        }

        NfTraceClose(WINEVENT_LEVEL_VERBOSE, "Close_File", TraceLoggingPointer(deviceObject),
                     TraceLoggingPointer(irpSp->FileObject, "fileObject"));
        rc = STATUS_SUCCESS;
    }
    FINALLY
    {
        return NfCompleteRequest(irp, rc, information);
    }
}
