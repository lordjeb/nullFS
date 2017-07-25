#pragma once
#include <ntifs.h>
#include "struct.h"


_Function_class_(IRP_MJ_CLEANUP)
_Function_class_(DRIVER_DISPATCH)
NTSTATUS NfFsdCleanup(_In_ PDEVICE_OBJECT volumeDeviceObject, _Inout_ PIRP irp);

_Function_class_(IRP_MJ_CLOSE)
_Function_class_(DRIVER_DISPATCH)
NTSTATUS NfFsdClose(_In_ PDEVICE_OBJECT volumeDeviceObject, _Inout_ PIRP irp);

_Function_class_(IRP_MJ_CREATE)
_Function_class_(DRIVER_DISPATCH)
NTSTATUS NfFsdCreate(_In_ PDEVICE_OBJECT volumeDeviceObject, _Inout_ PIRP irp);

_Function_class_(IRP_MJ_DEVICE_CONTROL)
_Function_class_(DRIVER_DISPATCH)
NTSTATUS NfFsdDeviceControl(_In_ PDEVICE_OBJECT volumeDeviceObject, _Inout_ PIRP irp);

_Function_class_(IRP_MJ_FILE_SYSTEM_CONTROL)
_Function_class_(DRIVER_DISPATCH)
NTSTATUS NfFsdFileSystemControl(_In_ PDEVICE_OBJECT volumeDeviceObject, _Inout_ PIRP irp);
