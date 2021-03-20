#include <ntifs.h>
#define WIL_KERNEL_MODE
#include <wil/resource.h>
#include <nullFS/names.h>
#include "struct.h"
#include "debug.h"
#include "flowControl.h"
#include "dispatchRoutines.h"

#pragma prefast(disable : __WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode")

// ---------------------------------------------------------------------------
// Global variables
//

NfGlobalData globalData;

// ---------------------------------------------------------------------------
// Driver unload functions
//

void NfUninitializeFileSystemDeviceObject()
{
    UNICODE_STRING symbolicLinkName;

    PAGED_CODE();

    if (FlagOn(globalData.flags, NF_GLOBAL_DATA_FLAGS_SYMBOLIC_LINK_CREATED))
    {
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
    UNICODE_STRING symbolicLinkName;

    PAGED_CODE();

    if (FlagOn(globalData.flags, NF_GLOBAL_DATA_FLAGS_DISK_SYMBOLIC_LINK_CREATED))
    {
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
    PAGED_CODE();

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

    NfDbgPrint(DPFLTR_INFO_LEVEL, "NfDriverUnload\n");

    NfUninitializeFileSystemDeviceObject();
    NfUninitializeDiskDeviceObject();
    NfUninitializeGlobals();
}

// ---------------------------------------------------------------------------
// Driver entry functions
//

NTSTATUS NfInitializeFileSystemDeviceObject()
{
    NTSTATUS rc;
    UNICODE_STRING driverDeviceName = RTL_CONSTANT_STRING(NF_DRIVER_DEVICE_NAME);
    UNICODE_STRING symbolicLinkName = RTL_CONSTANT_STRING(NF_DRIVER_SYMBOLIC_NAME);

    ASSERT(globalData.driverObject);

    rc = IoCreateDevice(globalData.driverObject, 0, &driverDeviceName, NF_DEVICE_TYPE, 0, FALSE,
                        &globalData.fileSystemDeviceObject);
    FUNCTION_EXIT_IF_NOT_SUCCESS(rc);
    SetFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_DRIVER_DEVICE_CREATED);

    rc = IoCreateSymbolicLink(&symbolicLinkName, &driverDeviceName);
    FUNCTION_EXIT_IF_NOT_SUCCESS(rc);
    SetFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_SYMBOLIC_LINK_CREATED);

    // Register our file system with the I/O subsystem (also adds a reference to the object)
    IoRegisterFileSystem(globalData.fileSystemDeviceObject);
    SetFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_FILE_SYSTEM_REGISTERED);

function_exit:

    return rc;
}

NTSTATUS NfInitializeDiskDeviceObject()
{
    NTSTATUS rc;
    UNICODE_STRING driverDeviceName = RTL_CONSTANT_STRING(NF_DRIVER_DISK_DEVICE_NAME);
    UNICODE_STRING symbolicLinkName = RTL_CONSTANT_STRING(NF_DRIVER_DISK_SYMBOLIC_NAME);

    ASSERT(globalData.driverObject);

    rc = IoCreateDevice(globalData.driverObject, 0, &driverDeviceName, FILE_DEVICE_DISK, 0, FALSE,
                        &globalData.diskDeviceObject);
    FUNCTION_EXIT_IF_NOT_SUCCESS(rc);
    SetFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_DISK_DRIVER_DEVICE_CREATED);

    rc = IoCreateSymbolicLink(&symbolicLinkName, &driverDeviceName);
    FUNCTION_EXIT_IF_NOT_SUCCESS(rc);
    SetFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_DISK_SYMBOLIC_LINK_CREATED);

function_exit:

    return rc;
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
    NTSTATUS rc;

    RtlZeroMemory(&globalData, sizeof(globalData));

    globalData.driverObject = driverObject;

    rc = ExInitializeResourceLite(&globalData.lock);
    FUNCTION_EXIT_IF_NOT_SUCCESS(rc);
    SetFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_RESOURCE_INITIALIZED);

function_exit:

    return rc;
}

NTSTATUS NfInitializeParameters(_In_ PDRIVER_OBJECT driverObject, _In_ PUNICODE_STRING registryPath)
{
    OBJECT_ATTRIBUTES oa = RTL_CONSTANT_OBJECT_ATTRIBUTES(registryPath, OBJ_CASE_INSENSITIVE);
    wil::unique_kernel_handle key;
    NTSTATUS rc = ZwOpenKey(key.addressof(), KEY_READ, &oa);
    FUNCTION_EXIT_IF_NOT_SUCCESS(rc);

    // Read break on load!
    // Read break on status (and integrate into error handling macros)

function_exit:

    return rc;
}

extern "C" NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT driverObject, _In_ PUNICODE_STRING registryPath)
{
    NTSTATUS rc = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(registryPath);

    NfDbgPrint(DPFLTR_INFO_LEVEL, "DriverEntry [Build timestamp: %s]\n", __TIMESTAMP__);
#if defined(DBG)
    DbgBreakPoint();
#endif

    rc = NfInitializeGlobals(driverObject);
    FUNCTION_EXIT_IF_NOT_SUCCESS(rc);

    rc = NfInitializeParameters(driverObject, registryPath);
    FUNCTION_EXIT_IF_NOT_SUCCESS(rc);

    // Initialize the driver object,
    NfInitializeFsdDispatch();
    // Allow unload for debugging purposes only
#if defined(DBG)
    driverObject->DriverUnload = NfDriverUnload;
#endif

    rc = NfInitializeDiskDeviceObject();
    FUNCTION_EXIT_IF_NOT_SUCCESS(rc);

    // Create the device object
    rc = NfInitializeFileSystemDeviceObject();
    FUNCTION_EXIT_IF_NOT_SUCCESS(rc);

function_exit:

    if (!NT_SUCCESS(rc))
    {
        NfDbgPrint(DPFLTR_ERROR_LEVEL, "DriverEntry failed (%08x)\n", rc);

        NfDriverUnload(driverObject);
    }

    return rc;
}
