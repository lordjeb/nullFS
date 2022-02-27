#include "pch.h"
#include "flowControl.h"
#include "dispatchRoutines.h"

_Dispatch_type_(IRP_MJ_SHUTDOWN) _Function_class_(IRP_MJ_SHUTDOWN) _Function_class_(DRIVER_DISPATCH) extern "C" NTSTATUS
    NfFsdShutdown(_In_ PDEVICE_OBJECT deviceObject, _Inout_ PIRP irp)
{
    NTSTATUS rc;
    TRY
    {
        // auto irpSp = IoGetCurrentIrpStackLocation(irp);

        NfTraceShutdown(WINEVENT_LEVEL_INFO, "Shutdown", TraceLoggingPointer(deviceObject));
        rc = STATUS_SUCCESS;
    }
    FINALLY
    {
        return NfCompleteRequest(irp, rc, 0);
    }
}
