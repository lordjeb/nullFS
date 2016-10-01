#include "common.h"
#include <ntifs.h>
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

void NfInitializeFsdDispatch(_In_ PDRIVER_OBJECT driverObject);

// ---------------------------------------------------------------------------
// Assign text sections for each routine.
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, NfDriverUnload)
#pragma alloc_text(INIT, NfInitializeFsdDispatch)
#endif


// ---------------------------------------------------------------------------
// Function implementations
//

NTSTATUS DriverEntry(
    _In_ PDRIVER_OBJECT driverObject,
    _In_ _Unreferenced_parameter_ PUNICODE_STRING registryPath
    )
{
    NTSTATUS rc = STATUS_SUCCESS;
    UNICODE_STRING driverDeviceName = RTL_CONSTANT_STRING(L"\\nullFS");

    UNREFERENCED_PARAMETER(registryPath);

    try
    {
        try
        {
            RtlZeroMemory(&globalData, sizeof(globalData));

            rc = ExInitializeResourceLite(&globalData.lock);
            ASSERT(NT_SUCCESS(rc));
            SetFlag(globalData.flags, GLOBAL_DATA_FLAGS_RESOURCE_INITIALIZED);

            globalData.driverObject = driverObject;

            // Initialize the device object
            driverObject->DriverUnload = NfDriverUnload;
            NfInitializeFsdDispatch(driverObject);

            // Create the device object
            rc = IoCreateDevice(driverObject, 0, &driverDeviceName, FILE_DEVICE_DISK_FILE_SYSTEM, 0, FALSE, &globalData.controlDeviceObject);
            if (!NT_SUCCESS(rc))
            {
                TRY_EXIT;
            }

            // Register our file system with the I/O subsystem (also adds a reference to the object)
            IoRegisterFileSystem(globalData.controlDeviceObject);
        }
        except (EXCEPTION_EXECUTE_HANDLER)
        {
            rc = GetExceptionCode();
        }

        try_exit: NOTHING;
    }
    finally
    {
        if (!NT_SUCCESS(rc))
        {
            if (FlagOn(globalData.flags, GLOBAL_DATA_FLAGS_RESOURCE_INITIALIZED))
            {
                ExDeleteResourceLite(&globalData.lock);
                ClearFlag(globalData.flags, GLOBAL_DATA_FLAGS_RESOURCE_INITIALIZED);
            }

            if (globalData.controlDeviceObject)
            {
                IoDeleteDevice(globalData.controlDeviceObject);
                globalData.controlDeviceObject = NULL;
            }
        }
    }

    return rc;
}

_Function_class_(DRIVER_UNLOAD) 
void NfDriverUnload(
    _In_ _Unreferenced_parameter_ PDRIVER_OBJECT driverObject
    )
{
    UNREFERENCED_PARAMETER(driverObject);

    if (FlagOn(globalData.flags, GLOBAL_DATA_FLAGS_RESOURCE_INITIALIZED))
    {
        ExDeleteResourceLite(&globalData.lock);
        ClearFlag(globalData.flags, GLOBAL_DATA_FLAGS_RESOURCE_INITIALIZED);
    }

    IoUnregisterFileSystem(globalData.controlDeviceObject);
    IoDeleteDevice(globalData.controlDeviceObject);
    globalData.controlDeviceObject = NULL;
}

void NfInitializeFsdDispatch(_In_ PDRIVER_OBJECT driverObject)
{
    driverObject->MajorFunction[IRP_MJ_CREATE] = (PDRIVER_DISPATCH)NfFsdCreate;
    driverObject->MajorFunction[IRP_MJ_CLOSE] = (PDRIVER_DISPATCH)NfFsdClose;
    //driverObject->MajorFunction[IRP_MJ_READ] = (PDRIVER_DISPATCH)FatFsdRead;
    //driverObject->MajorFunction[IRP_MJ_WRITE] = (PDRIVER_DISPATCH)FatFsdWrite;
    //driverObject->MajorFunction[IRP_MJ_QUERY_INFORMATION] = (PDRIVER_DISPATCH)FatFsdQueryInformation;
    //driverObject->MajorFunction[IRP_MJ_SET_INFORMATION] = (PDRIVER_DISPATCH)FatFsdSetInformation;
    //driverObject->MajorFunction[IRP_MJ_QUERY_EA] = (PDRIVER_DISPATCH)FatFsdQueryEa;
    //driverObject->MajorFunction[IRP_MJ_SET_EA] = (PDRIVER_DISPATCH)FatFsdSetEa;
    //driverObject->MajorFunction[IRP_MJ_FLUSH_BUFFERS] = (PDRIVER_DISPATCH)FatFsdFlushBuffers;
    //driverObject->MajorFunction[IRP_MJ_QUERY_VOLUME_INFORMATION] = (PDRIVER_DISPATCH)FatFsdQueryVolumeInformation;
    //driverObject->MajorFunction[IRP_MJ_SET_VOLUME_INFORMATION] = (PDRIVER_DISPATCH)FatFsdSetVolumeInformation;
    driverObject->MajorFunction[IRP_MJ_CLEANUP] = (PDRIVER_DISPATCH)NfFsdCleanup;
    //driverObject->MajorFunction[IRP_MJ_DIRECTORY_CONTROL] = (PDRIVER_DISPATCH)FatFsdDirectoryControl;
    //driverObject->MajorFunction[IRP_MJ_FILE_SYSTEM_CONTROL] = (PDRIVER_DISPATCH)FatFsdFileSystemControl;
    //driverObject->MajorFunction[IRP_MJ_LOCK_CONTROL] = (PDRIVER_DISPATCH)FatFsdLockControl;
    //driverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = (PDRIVER_DISPATCH)FatFsdDeviceControl;
    //driverObject->MajorFunction[IRP_MJ_SHUTDOWN] = (PDRIVER_DISPATCH)FatFsdShutdown;
    //driverObject->MajorFunction[IRP_MJ_PNP] = (PDRIVER_DISPATCH)FatFsdPnp;
}
