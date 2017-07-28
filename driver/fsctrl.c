#include "common.h"
#include <ntifs.h>

// ---------------------------------------------------------------------------
// Assign text sections for each routine.
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, NfFsdFileSystemControl)
#endif

// ---------------------------------------------------------------------------
// Function implementations
//

_Function_class_(IRP_MJ_FILE_SYSTEM_CONTROL)
_Function_class_(DRIVER_DISPATCH)
NTSTATUS NfFsdFileSystemControl(_In_ PDEVICE_OBJECT volumeDeviceObject, _Inout_ PIRP irp)
{
	NTSTATUS rc = STATUS_ILLEGAL_FUNCTION;

	NfDbgPrint(DPFLTR_FS_CONTROL, "nullFS: IRP_MJ_FILE_SYSTEM_CONTROL\n");

	if (NfDeviceIsFileSystemDeviceObject((PDEVICE_OBJECT) volumeDeviceObject))
	{
		PIO_STACK_LOCATION currentIrpStackLocation = IoGetCurrentIrpStackLocation(irp);

		switch (currentIrpStackLocation->MinorFunction)
		{
			case IRP_MN_MOUNT_VOLUME:
				NfDbgPrint(DPFLTR_FS_CONTROL, "nullFS: IRP_MN_MOUNT_VOLUME\n");
				break;
		}

		FUNCTION_EXIT;
	}

	if (NfDeviceIsDiskDeviceObject((PDEVICE_OBJECT) volumeDeviceObject))
	{
		NfDbgPrint(DPFLTR_FS_CONTROL, "nullFS: Volume device object\n");
		FUNCTION_EXIT;
	}

	NfDbgPrint(DPFLTR_FS_CONTROL, "nullFS: Unrecognized device object\n");

function_exit:

	return NfCompleteRequest(irp, rc, 0);
}
