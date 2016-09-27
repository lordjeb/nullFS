#include "nullfs.h"
#include <ntifs.h>

// ---------------------------------------------------------------------------
// Assign text sections for each routine.
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, FsdClose)
#endif

// ---------------------------------------------------------------------------
// Function implementations
//

_Function_class_(IRP_MJ_CLOSE)
_Function_class_(DRIVER_DISPATCH)
NTSTATUS FsdClose(
	_In_ PVOLUME_DEVICE_OBJECT volumeDeviceObject,
	_Inout_ PIRP irp
	)
{
	UNREFERENCED_PARAMETER(volumeDeviceObject);
	UNREFERENCED_PARAMETER(irp);

	return STATUS_SUCCESS;
}
