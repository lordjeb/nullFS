#include "pch.h"
#define WIL_KERNEL_MODE
#include <wil/resource.h>
#include <nullFS/names.h>
#include "struct.h"
#include "dispatchRoutines.h"

#pragma prefast(disable : __WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode")

using unique_registry_parameter_data =
    wil::unique_tagged_pool_ptr<KEY_VALUE_PARTIAL_INFORMATION*, TAG_REGISTRY_PARAMETER>;

// ---------------------------------------------------------------------------
// Global variables
//

NfGlobalData globalData;

// ---------------------------------------------------------------------------
// Driver unload functions
//

void NfUninitializeFileSystemDeviceObject()
{
    if (FlagOn(globalData.flags, NF_GLOBAL_DATA_FLAGS_SYMBOLIC_LINK_CREATED))
    {
        UNICODE_STRING symbolicLinkName;
        ClearFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_SYMBOLIC_LINK_CREATED);
        RtlInitUnicodeString(&symbolicLinkName, NF_DRIVER_SYMBOLIC_NAME);
        IoDeleteSymbolicLink(&symbolicLinkName);
    }

    if (FlagOn(globalData.flags, NF_GLOBAL_DATA_FLAGS_DRIVER_DEVICE_CREATED))
    {
        ClearFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_DRIVER_DEVICE_CREATED);
        IoDeleteDevice(globalData.fileSystemDeviceObject);
        globalData.fileSystemDeviceObject = nullptr;
    }
}

void NfUninitializeDiskDeviceObject()
{
    if (FlagOn(globalData.flags, NF_GLOBAL_DATA_FLAGS_DISK_SYMBOLIC_LINK_CREATED))
    {
        UNICODE_STRING symbolicLinkName;
        ClearFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_DISK_SYMBOLIC_LINK_CREATED);
        RtlInitUnicodeString(&symbolicLinkName, NF_DRIVER_DISK_SYMBOLIC_NAME);
        IoDeleteSymbolicLink(&symbolicLinkName);
    }

    if (FlagOn(globalData.flags, NF_GLOBAL_DATA_FLAGS_DISK_DRIVER_DEVICE_CREATED))
    {
        ClearFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_DISK_DRIVER_DEVICE_CREATED);
        IoDeleteDevice(globalData.diskDeviceObject);
        globalData.diskDeviceObject = nullptr;
    }
}

void NfUninitializeGlobals()
{
    if (FlagOn(globalData.flags, NF_GLOBAL_DATA_FLAGS_RESOURCE_INITIALIZED))
    {
        ClearFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_RESOURCE_INITIALIZED);
        ExDeleteResourceLite(&globalData.lock);
    }
}

_Function_class_(DRIVER_UNLOAD) void NfDriverUnload(_In_ PDRIVER_OBJECT driverObject)
{
    UNREFERENCED_PARAMETER(driverObject);
    PAGED_CODE();

    NfDbgPrint(DPFLTR_DRIVER_ENTRY, "NfDriverUnload\n");

    NfUninitializeFileSystemDeviceObject();
    NfUninitializeDiskDeviceObject();
    NfUninitializeGlobals();
}

// ---------------------------------------------------------------------------
// Driver entry functions
//

NTSTATUS NfInitializeFileSystemDeviceObject()
{
    ASSERT(globalData.driverObject);

    NTSTATUS rc{ STATUS_SUCCESS };
    __try
    {
        UNICODE_STRING driverDeviceName = RTL_CONSTANT_STRING(NF_DRIVER_DEVICE_NAME);
        rc = IoCreateDevice(globalData.driverObject, 0, &driverDeviceName, NF_DEVICE_TYPE, 0, FALSE,
                            &globalData.fileSystemDeviceObject);
        LEAVE_IF_NOT_SUCCESS(rc);
        SetFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_DRIVER_DEVICE_CREATED);

        UNICODE_STRING symbolicLinkName = RTL_CONSTANT_STRING(NF_DRIVER_SYMBOLIC_NAME);
        rc = IoCreateSymbolicLink(&symbolicLinkName, &driverDeviceName);
        LEAVE_IF_NOT_SUCCESS(rc);
        SetFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_SYMBOLIC_LINK_CREATED);

        // Register our file system with the I/O subsystem (also adds a reference to the object)
        IoRegisterFileSystem(globalData.fileSystemDeviceObject);
        SetFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_FILE_SYSTEM_REGISTERED);
    }
    __finally
    {
        return rc;
    }
}

NTSTATUS NfInitializeDiskDeviceObject()
{
    ASSERT(globalData.driverObject);

    NTSTATUS rc{ STATUS_SUCCESS };
    __try
    {
        UNICODE_STRING driverDeviceName = RTL_CONSTANT_STRING(NF_DRIVER_DISK_DEVICE_NAME);
        rc = IoCreateDevice(globalData.driverObject, 0, &driverDeviceName, FILE_DEVICE_DISK, 0, FALSE,
                            &globalData.diskDeviceObject);
        LEAVE_IF_NOT_SUCCESS(rc);
        SetFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_DISK_DRIVER_DEVICE_CREATED);

        UNICODE_STRING symbolicLinkName = RTL_CONSTANT_STRING(NF_DRIVER_DISK_SYMBOLIC_NAME);
        rc = IoCreateSymbolicLink(&symbolicLinkName, &driverDeviceName);
        LEAVE_IF_NOT_SUCCESS(rc);
        SetFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_DISK_SYMBOLIC_LINK_CREATED);
    }
    __finally
    {
        return rc;
    }
}

void NfInitializeFsdDispatch()
{
    ASSERT(globalData.driverObject);

#pragma warning(push)
#pragma warning(disable : 28175)
    globalData.driverObject->MajorFunction[IRP_MJ_CREATE] = (PDRIVER_DISPATCH)NfFsdCreate;
    globalData.driverObject->MajorFunction[IRP_MJ_CLOSE] = (PDRIVER_DISPATCH)NfFsdClose;
    // globalData.driverObject->MajorFunction[IRP_MJ_READ] = (PDRIVER_DISPATCH)NfFsdRead;
    // globalData.driverObject->MajorFunction[IRP_MJ_WRITE] = (PDRIVER_DISPATCH)NfFsdWrite;
    // globalData.driverObject->MajorFunction[IRP_MJ_QUERY_INFORMATION] = (PDRIVER_DISPATCH)NfFsdQueryInformation;
    // globalData.driverObject->MajorFunction[IRP_MJ_SET_INFORMATION] = (PDRIVER_DISPATCH)NfFsdSetInformation;
    // globalData.driverObject->MajorFunction[IRP_MJ_QUERY_EA] = (PDRIVER_DISPATCH)NfFsdQueryEa;
    // globalData.driverObject->MajorFunction[IRP_MJ_SET_EA] = (PDRIVER_DISPATCH)NfFsdSetEa;
    // globalData.driverObject->MajorFunction[IRP_MJ_FLUSH_BUFFERS] = (PDRIVER_DISPATCH)NfFsdFlushBuffers;
    // globalData.driverObject->MajorFunction[IRP_MJ_QUERY_VOLUME_INFORMATION] =
    // (PDRIVER_DISPATCH)NfFsdQueryVolumeInformation;
    // globalData.driverObject->MajorFunction[IRP_MJ_SET_VOLUME_INFORMATION] =
    // (PDRIVER_DISPATCH)NfFsdSetVolumeInformation;
    globalData.driverObject->MajorFunction[IRP_MJ_CLEANUP] = (PDRIVER_DISPATCH)NfFsdCleanup;
    // globalData.driverObject->MajorFunction[IRP_MJ_DIRECTORY_CONTROL] = (PDRIVER_DISPATCH)NfFsdDirectoryControl;
    // globalData.driverObject->MajorFunction[IRP_MJ_FILE_SYSTEM_CONTROL] = (PDRIVER_DISPATCH)NfFsdFileSystemControl;
    // globalData.driverObject->MajorFunction[IRP_MJ_LOCK_CONTROL] = (PDRIVER_DISPATCH)NfFsdLockControl;
    globalData.driverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = (PDRIVER_DISPATCH)NfFsdDeviceControl;
    // globalData.driverObject->MajorFunction[IRP_MJ_SHUTDOWN] = (PDRIVER_DISPATCH)NfFsdShutdown;
    // globalData.driverObject->MajorFunction[IRP_MJ_PNP] = (PDRIVER_DISPATCH)NfFsdPnp;
#pragma warning(pop)
}

NTSTATUS NfInitializeGlobals(_In_ PDRIVER_OBJECT driverObject)
{
    NTSTATUS rc{ STATUS_SUCCESS };
    __try
    {
        RtlZeroMemory(&globalData, sizeof(globalData));

        globalData.driverObject = driverObject;

        rc = ExInitializeResourceLite(&globalData.lock);
        LEAVE_IF_NOT_SUCCESS(rc);
        SetFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_RESOURCE_INITIALIZED);
    }
    __finally
    {
        return rc;
    }
}

NTSTATUS NfInitializeParameters(_In_ UNICODE_STRING* registryPath)
{
    NTSTATUS rc{ STATUS_SUCCESS };
    __try
    {
        OBJECT_ATTRIBUTES oaDriverKey = RTL_CONSTANT_OBJECT_ATTRIBUTES(registryPath, OBJ_CASE_INSENSITIVE);
        wil::unique_kernel_handle driverKey;
        rc = ZwOpenKey(driverKey.addressof(), KEY_READ, &oaDriverKey);
        if (STATUS_SUCCESS == rc)
        {
            UNICODE_STRING parametersKeyName = RTL_CONSTANT_STRING(L"Parameters");
            OBJECT_ATTRIBUTES oaParametersKey{ sizeof(OBJECT_ATTRIBUTES),
                                               driverKey.get(),
                                               RTL_CONST_CAST(PUNICODE_STRING)(&parametersKeyName),
                                               OBJ_CASE_INSENSITIVE,
                                               NULL,
                                               NULL };
            wil::unique_kernel_handle parametersKey;
            rc = ZwOpenKey(parametersKey.addressof(), KEY_READ, &oaParametersKey);
            if (STATUS_SUCCESS == rc)
            {
                ULONG cbKvpi = FIELD_OFFSET(KEY_VALUE_PARTIAL_INFORMATION, Data) + sizeof(ULONG);
                unique_registry_parameter_data kvpi{ static_cast<KEY_VALUE_PARTIAL_INFORMATION*>(
                    ExAllocatePool2(POOL_FLAG_PAGED, cbKvpi, TAG_REGISTRY_PARAMETER)) };
                if (!kvpi.is_valid())
                {
                    LEAVE_WITH(rc = STATUS_INSUFFICIENT_RESOURCES);
                }

                UNICODE_STRING breakOnLoadValueName = RTL_CONSTANT_STRING(L"BreakOnLoad");
                ULONG returnLength;
                rc = ZwQueryValueKey(parametersKey.get(), &breakOnLoadValueName, KeyValuePartialInformation, kvpi.get(),
                                     cbKvpi, &returnLength);
                if (STATUS_SUCCESS == rc)
                {
                    globalData.Parameters.BreakOnLoad = (0 != *(&kvpi.get()->Data[0]));
                }

                UNICODE_STRING breakOnNtStatusValueName = RTL_CONSTANT_STRING(L"BreakOnNtStatus");
                rc = ZwQueryValueKey(parametersKey.get(), &breakOnNtStatusValueName, KeyValuePartialInformation,
                                     kvpi.get(), cbKvpi, &returnLength);
                if (STATUS_SUCCESS == rc)
                {
                    globalData.Parameters.BreakOnNtStatus = *(&kvpi.get()->Data[0]);
                }
            }
        }

        rc = STATUS_SUCCESS;
    }
    __finally
    {
        return rc;
    }
}

extern "C" NTSTATUS DriverEntry(_In_ DRIVER_OBJECT* driverObject, _In_ UNICODE_STRING* registryPath)
{
    NTSTATUS rc{ STATUS_SUCCESS };
    __try
    {
        UNREFERENCED_PARAMETER(registryPath);

        NfDbgPrint(DPFLTR_DRIVER_ENTRY, "DriverEntry [Build timestamp: %s]\n", __TIMESTAMP__);

        rc = NfInitializeGlobals(driverObject);
        LEAVE_IF_NOT_SUCCESS(rc);

        rc = NfInitializeParameters(registryPath);
        LEAVE_IF_NOT_SUCCESS(rc);

        if (globalData.Parameters.BreakOnLoad)
        {
            DbgBreakPoint();
        }

        // Initialize the driver object,
        NfInitializeFsdDispatch();
        // Allow unload for debugging purposes only
#if defined(DBG)
        driverObject->DriverUnload = NfDriverUnload;
#endif

        rc = NfInitializeDiskDeviceObject();
        LEAVE_IF_NOT_SUCCESS(rc);

        // Create the device object
        rc = NfInitializeFileSystemDeviceObject();
        LEAVE_IF_NOT_SUCCESS(rc);
    }
    __finally
    {
        if (!NT_SUCCESS(rc))
        {
            NfDbgPrint(DPFLTR_DRIVER_ENTRY, "DriverEntry failed (%08x)\n", rc);

            NfDriverUnload(driverObject);
        }

        return rc;
    }
}
