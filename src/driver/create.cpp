#include "pch.h"
#include "flowControl.h"
#include "dispatchRoutines.h"

// ---------------------------------------------------------------------------
// Function implementations
//

_Dispatch_type_(IRP_MJ_CREATE) _Function_class_(IRP_MJ_CREATE) _Function_class_(DRIVER_DISPATCH) extern "C" NTSTATUS
    NfFsdCreate(_In_ PDEVICE_OBJECT volumeDeviceObject, _Inout_ PIRP irp)
{
    PAGED_CODE();

    NTSTATUS rc{ STATUS_ILLEGAL_FUNCTION };
    TRY
    {
        ULONG_PTR information = 0;   // TODO: What to do with this?
        PIO_STACK_LOCATION currentIrpStackLocation = IoGetCurrentIrpStackLocation(irp);

        NfDbgPrint(DPFLTR_CREATE, "%s: [DevObj=%08p,FileObj=%08p]\n", __FUNCTION__, volumeDeviceObject,
                   currentIrpStackLocation->FileObject);

        // If we were called with our file system device object instead of a volume device object, just return success

        if (NfDeviceIsFileSystemDeviceObject(volumeDeviceObject))
        {
            NfDbgPrint(DPFLTR_CREATE, "%s: FileSystemDO\n", __FUNCTION__);
            information = FILE_OPENED;
            LEAVE_WITH(rc = STATUS_SUCCESS);
        }

        // TODO: Open the file object based on the name
        NfDbgPrint(DPFLTR_CREATE, "%s: VolumeDO\n", __FUNCTION__);
        LEAVE_WITH(rc = STATUS_OBJECT_NAME_NOT_FOUND);
    }
    FINALLY
    {
        return NfCompleteRequest(irp, rc, 0);
    }
}
