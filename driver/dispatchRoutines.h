#pragma once
#include <ntifs.h>
#include "struct.h"


_Function_class_(IRP_MJ_CLEANUP)
_Function_class_(DRIVER_DISPATCH)
NTSTATUS
FsdCleanup(
	_In_ PVOLUME_DEVICE_OBJECT volumeDeviceObject,
	_Inout_ PIRP irp
	);

_Function_class_(IRP_MJ_CLOSE)
_Function_class_(DRIVER_DISPATCH)
NTSTATUS
FsdClose(
	_In_ PVOLUME_DEVICE_OBJECT volumeDeviceObject,
	_Inout_ PIRP irp
	);

_Function_class_(IRP_MJ_CREATE)
_Function_class_(DRIVER_DISPATCH)
NTSTATUS
FsdCreate(
	_In_ PVOLUME_DEVICE_OBJECT volumeDeviceObject,
	_Inout_ PIRP irp
	);
