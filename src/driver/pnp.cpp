#include "pch.h"
#include "flowControl.h"
#include "dispatchRoutines.h"

_Dispatch_type_(IRP_MJ_PNP) _Function_class_(IRP_MJ_PNP) _Function_class_(DRIVER_DISPATCH) extern "C" NTSTATUS
    NfFsdPnp(_In_ PDEVICE_OBJECT deviceObject, _Inout_ PIRP irp)
{
    PAGED_CODE();

    NTSTATUS rc{ STATUS_NOT_IMPLEMENTED };
    TRY
    {
        auto irpSp = IoGetCurrentIrpStackLocation(irp);

        switch (irpSp->MinorFunction)
        {
        case IRP_MN_SURPRISE_REMOVAL:
            NfTracePnp(WINEVENT_LEVEL_INFO, "SurpriseRemoval", TraceLoggingPointer(deviceObject));
            rc = STATUS_SUCCESS;
            break;

        default:
            NfTracePnp(WINEVENT_LEVEL_INFO, "UnhandledMinorFn", TraceLoggingPointer(deviceObject),
                       TraceLoggingUInt8(irpSp->MinorFunction, "MinorFunction"));
            break;
        }
    }
    FINALLY
    {
        return NfCompleteRequest(irp, rc, 0);
    }
}
