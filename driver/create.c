#include "common.h"
#include <ntifs.h>

// ---------------------------------------------------------------------------
// Assign text sections for each routine.
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, NfFsdCreate)
#endif

// ---------------------------------------------------------------------------
// Function implementations
//

_Function_class_(IRP_MJ_CREATE)
_Function_class_(DRIVER_DISPATCH)
NTSTATUS NfFsdCreate(_In_ PDEVICE_OBJECT volumeDeviceObject, _Inout_ PIRP irp)
{
	NTSTATUS rc = STATUS_ILLEGAL_FUNCTION;
	ULONG_PTR info = 0;

	NfDbgPrint(DPFLTR_CREATE, "nullFS: IRP_MJ_CREATE\n");

	if (NfDeviceIsFileSystemDeviceObject((PDEVICE_OBJECT) volumeDeviceObject) ||
		NfDeviceIsDiskDeviceObject((PDEVICE_OBJECT) volumeDeviceObject))
	{
		rc = STATUS_SUCCESS;
		info = FILE_OPENED;
		FUNCTION_EXIT;
	}

	NfDbgPrint(DPFLTR_CREATE, "nullFS: Unrecognized device object\n");

function_exit:

	return NfCompleteRequest(irp, rc, info);
}
