#pragma once
#include <ntifs.h>

extern "C" {

_Dispatch_type_(IRP_MJ_CLEANUP) _Function_class_(IRP_MJ_CLEANUP) _Function_class_(DRIVER_DISPATCH) NTSTATUS
    NfFsdCleanup(_In_ PDEVICE_OBJECT deviceObject, _Inout_ PIRP irp);

_Dispatch_type_(IRP_MJ_CLOSE) _Function_class_(IRP_MJ_CLOSE) _Function_class_(DRIVER_DISPATCH) NTSTATUS
    NfFsdClose(_In_ PDEVICE_OBJECT deviceObject, _Inout_ PIRP irp);

_Dispatch_type_(IRP_MJ_CREATE) _Function_class_(IRP_MJ_CREATE) _Function_class_(DRIVER_DISPATCH) NTSTATUS
    NfFsdCreate(_In_ PDEVICE_OBJECT deviceObject, _Inout_ PIRP irp);

_Dispatch_type_(IRP_MJ_DEVICE_CONTROL) _Function_class_(IRP_MJ_DEVICE_CONTROL)
    _Function_class_(DRIVER_DISPATCH) NTSTATUS NfFsdDeviceControl(_In_ PDEVICE_OBJECT deviceObject, _Inout_ PIRP irp);

_Dispatch_type_(IRP_MJ_FILE_SYSTEM_CONTROL) _Function_class_(IRP_MJ_FILE_SYSTEM_CONTROL)
    _Function_class_(DRIVER_DISPATCH) NTSTATUS
    NfFsdFileSystemControl(_In_ PDEVICE_OBJECT deviceObject, _Inout_ PIRP irp);

_Dispatch_type_(IRP_MJ_PNP) _Function_class_(IRP_MJ_PNP) _Function_class_(DRIVER_DISPATCH) NTSTATUS
    NfFsdPnp(_In_ PDEVICE_OBJECT deviceObject, _Inout_ PIRP irp);

_Dispatch_type_(IRP_MJ_SHUTDOWN) _Function_class_(IRP_MJ_SHUTDOWN) _Function_class_(DRIVER_DISPATCH) NTSTATUS
    NfFsdShutdown(_In_ PDEVICE_OBJECT deviceObject, _Inout_ PIRP irp);
}
