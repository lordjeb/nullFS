#include "pch.h"
#include "flowControl.h"
#include "dispatchRoutines.h"

// ---------------------------------------------------------------------------
// Function implementations
//

_Dispatch_type_(IRP_MJ_CREATE) _Function_class_(IRP_MJ_CREATE) _Function_class_(DRIVER_DISPATCH) extern "C" NTSTATUS
    NfFsdCreate(_In_ PDEVICE_OBJECT deviceObject, _Inout_ PIRP irp)
{
    NTSTATUS rc{ STATUS_NOT_IMPLEMENTED };
    ULONG_PTR information{ 0 };
    TRY
    {
        const auto irpSp = IoGetCurrentIrpStackLocation(irp);

        // If we were called with our file system device object instead of a volume device object just complete with
        // STATUS_SUCCESS

        if (NfDeviceIsFileSystemDeviceObject(deviceObject))
        {
            NfTraceCreate(WINEVENT_LEVEL_VERBOSE, "Fsdo", TraceLoggingPointer(deviceObject));
            information = FILE_OPENED;
            LEAVE_WITH(rc = STATUS_SUCCESS);
        }

        // TODO: Open the file object based on the name
        NfTraceCreate(WINEVENT_LEVEL_VERBOSE, "File", TraceLoggingPointer(deviceObject),
                      TraceLoggingPointer(irpSp->FileObject, "fileObject"),
                      TraceLoggingUnicodeString(&(irpSp->FileObject->FileName), "fileName"));
        LEAVE_WITH(rc = STATUS_OBJECT_NAME_NOT_FOUND);
    }
    FINALLY
    {
        return NfCompleteRequest(irp, rc, information);
    }
}
