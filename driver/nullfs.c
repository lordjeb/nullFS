#include "common.h"
#include <ntifs.h>
#include "names.h"
//#include <dontuse.h>
//#include <suppress.h>

#pragma prefast(disable:__WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode")


// ---------------------------------------------------------------------------
// Global variables
//

NfGlobalData globalData;

// ---------------------------------------------------------------------------
// Function prototypes
//

DRIVER_INITIALIZE DriverEntry;

NTSTATUS DriverEntry(
    _In_ PDRIVER_OBJECT driverObject,
    _In_ _Unreferenced_parameter_ PUNICODE_STRING registryPath
    );

_Function_class_(DRIVER_UNLOAD)
void NfDriverUnload(
    _In_ _Unreferenced_parameter_ PDRIVER_OBJECT driverObject
    );

NTSTATUS NfInitializeDiskDeviceObject(_In_ PDRIVER_OBJECT driverObject);

NTSTATUS NfInitializeControlDeviceObject(_In_ PDRIVER_OBJECT driverObject);

void NfInitializeFsdDispatch(_In_ PDRIVER_OBJECT driverObject);

// ---------------------------------------------------------------------------
// Assign text sections for each routine.
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, NfDriverUnload)
#pragma alloc_text(INIT, NfInitializeFsdDispatch)
#pragma alloc_text(INIT, NfInitializeControlDeviceObject)
#endif


// ---------------------------------------------------------------------------
// Function implementations
//

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT driverObject, _In_ _Unreferenced_parameter_ PUNICODE_STRING registryPath)
{
    NTSTATUS rc = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(registryPath);

    KdPrint(("nullFS: DriverEntry [%s]\n", __TIMESTAMP__));

    RtlZeroMemory(&globalData, sizeof(globalData));

    globalData.driverObject = driverObject;

    rc = ExInitializeResourceLite(&globalData.lock);
    if (!NT_SUCCESS(rc))
    {
        FUNCTION_EXIT;
    }
    SetFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_RESOURCE_INITIALIZED);

    // Initialize the driver object
    NfInitializeFsdDispatch(driverObject);
#ifdef DBG
    driverObject->DriverUnload = NfDriverUnload;
#endif

    rc = NfInitializeDiskDeviceObject(driverObject);
    if (!NT_SUCCESS(rc))
    {
        FUNCTION_EXIT;
    }

    // Create the device object
    rc = NfInitializeControlDeviceObject(driverObject);
    if (!NT_SUCCESS(rc))
    {
        FUNCTION_EXIT;
    }

function_exit:

    if (!NT_SUCCESS(rc))
    {
        KdPrint(("nullFS: DriverEntry failed (%08x)\n", rc));

        NfDriverUnload(driverObject);
    }

    return rc;
}

_Function_class_(DRIVER_UNLOAD) 
void NfDriverUnload(_In_ _Unreferenced_parameter_ PDRIVER_OBJECT driverObject)
{
    UNICODE_STRING symbolicLinkName;

    UNREFERENCED_PARAMETER(driverObject);

    KdPrint(("nullFS: NfDriverUnload\n"));

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
        globalData.fileSystemDeviceObject = NULL;
    }

    if (FlagOn(globalData.flags, NF_GLOBAL_DATA_FLAGS_DISK_SYMBOLIC_LINK_CREATED))
    {
        ClearFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_DISK_SYMBOLIC_LINK_CREATED);
        RtlInitUnicodeString(&symbolicLinkName, NF_DRIVER_DISK_SYMBOLIC_NAME);
        IoDeleteSymbolicLink(&symbolicLinkName);
    }

    if (FlagOn(globalData.flags, NF_GLOBAL_DATA_FLAGS_DISK_SYMBOLIC_LINK_CREATED))
    {
        ClearFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_DISK_SYMBOLIC_LINK_CREATED);
        IoDeleteDevice(globalData.diskDeviceObject);
        globalData.diskDeviceObject = NULL;
    }

    if (FlagOn(globalData.flags, NF_GLOBAL_DATA_FLAGS_RESOURCE_INITIALIZED))
    {
        ClearFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_RESOURCE_INITIALIZED);
        ExDeleteResourceLite(&globalData.lock);
    }
}

NTSTATUS NfInitializeDiskDeviceObject(_In_ PDRIVER_OBJECT driverObject)
{
    NTSTATUS rc;
    UNICODE_STRING driverDeviceName = RTL_CONSTANT_STRING(NF_DRIVER_DISK_DEVICE_NAME);
    UNICODE_STRING symbolicLinkName = RTL_CONSTANT_STRING(NF_DRIVER_DISK_SYMBOLIC_NAME);

    rc = IoCreateDevice(driverObject, 0, &driverDeviceName, FILE_DEVICE_DISK, 0, FALSE, &globalData.diskDeviceObject);
    if (!NT_SUCCESS(rc))
    {
        FUNCTION_EXIT;
    }
    SetFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_DISK_DRIVER_DEVICE_CREATED);

    rc = IoCreateSymbolicLink(&symbolicLinkName, &driverDeviceName);
    if (!NT_SUCCESS(rc))
    {
        FUNCTION_EXIT;
    }
    SetFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_DISK_SYMBOLIC_LINK_CREATED);

function_exit:

    return rc;
}

NTSTATUS NfInitializeControlDeviceObject(_In_ PDRIVER_OBJECT driverObject)
{
    NTSTATUS rc;
    UNICODE_STRING driverDeviceName = RTL_CONSTANT_STRING(NF_DRIVER_DEVICE_NAME);
    UNICODE_STRING symbolicLinkName = RTL_CONSTANT_STRING(NF_DRIVER_SYMBOLIC_NAME);

    rc = IoCreateDevice(driverObject, 0, &driverDeviceName, NF_DEVICE_TYPE, 0, FALSE, &globalData.fileSystemDeviceObject);
    if (!NT_SUCCESS(rc))
    {
        FUNCTION_EXIT;
    }
    SetFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_DRIVER_DEVICE_CREATED);

    rc = IoCreateSymbolicLink(&symbolicLinkName, &driverDeviceName);
    if (!NT_SUCCESS(rc))
    {
        FUNCTION_EXIT;
    }
    SetFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_SYMBOLIC_LINK_CREATED);

    // Register our file system with the I/O subsystem (also adds a reference to the object)
    IoRegisterFileSystem(globalData.fileSystemDeviceObject);
    SetFlag(globalData.flags, NF_GLOBAL_DATA_FLAGS_FILE_SYSTEM_REGISTERED);

function_exit:

    return rc;
}

void NfInitializeFsdDispatch(_In_ PDRIVER_OBJECT driverObject)
{
    driverObject->MajorFunction[IRP_MJ_CREATE] = (PDRIVER_DISPATCH)NfFsdCreate;
    driverObject->MajorFunction[IRP_MJ_CLOSE] = (PDRIVER_DISPATCH)NfFsdClose;
    //driverObject->MajorFunction[IRP_MJ_READ] = (PDRIVER_DISPATCH)NfFsdRead;
    //driverObject->MajorFunction[IRP_MJ_WRITE] = (PDRIVER_DISPATCH)NfFsdWrite;
    //driverObject->MajorFunction[IRP_MJ_QUERY_INFORMATION] = (PDRIVER_DISPATCH)NfFsdQueryInformation;
    //driverObject->MajorFunction[IRP_MJ_SET_INFORMATION] = (PDRIVER_DISPATCH)NfFsdSetInformation;
    //driverObject->MajorFunction[IRP_MJ_QUERY_EA] = (PDRIVER_DISPATCH)NfFsdQueryEa;
    //driverObject->MajorFunction[IRP_MJ_SET_EA] = (PDRIVER_DISPATCH)NfFsdSetEa;
    //driverObject->MajorFunction[IRP_MJ_FLUSH_BUFFERS] = (PDRIVER_DISPATCH)NfFsdFlushBuffers;
    //driverObject->MajorFunction[IRP_MJ_QUERY_VOLUME_INFORMATION] = (PDRIVER_DISPATCH)NfFsdQueryVolumeInformation;
    //driverObject->MajorFunction[IRP_MJ_SET_VOLUME_INFORMATION] = (PDRIVER_DISPATCH)NfFsdSetVolumeInformation;
    driverObject->MajorFunction[IRP_MJ_CLEANUP] = (PDRIVER_DISPATCH)NfFsdCleanup;
    //driverObject->MajorFunction[IRP_MJ_DIRECTORY_CONTROL] = (PDRIVER_DISPATCH)NfFsdDirectoryControl;
    driverObject->MajorFunction[IRP_MJ_FILE_SYSTEM_CONTROL] = (PDRIVER_DISPATCH)NfFsdFileSystemControl;
    //driverObject->MajorFunction[IRP_MJ_LOCK_CONTROL] = (PDRIVER_DISPATCH)NfFsdLockControl;
    driverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = (PDRIVER_DISPATCH)NfFsdDeviceControl;
    //driverObject->MajorFunction[IRP_MJ_SHUTDOWN] = (PDRIVER_DISPATCH)NfFsdShutdown;
    //driverObject->MajorFunction[IRP_MJ_PNP] = (PDRIVER_DISPATCH)NfFsdPnp;
}
