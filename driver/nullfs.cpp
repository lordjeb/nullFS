#include "nullfs.h"
#include <ntifs.h>
//#include <dontuse.h>
//#include <suppress.h>

#pragma prefast(disable:__WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode")


// ---------------------------------------------------------------------------
// Global variables
//

PDEVICE_OBJECT NullFsDeviceObject = nullptr;


// ---------------------------------------------------------------------------
// Function prototypes
//

NULLFS DRIVER_INITIALIZE DriverEntry;
NULLFS NTSTATUS DriverEntry(
    _In_ _Unreferenced_parameter_ PDRIVER_OBJECT driverObject,
    _In_ _Unreferenced_parameter_ PUNICODE_STRING registryPath
    );

_Function_class_(DRIVER_UNLOAD)
NULLFS void DriverUnload(
    _In_ _Unreferenced_parameter_ PDRIVER_OBJECT driverObject
    );


// ---------------------------------------------------------------------------
// Assign text sections for each routine.
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, DriverUnload)
#endif


// ---------------------------------------------------------------------------
// Function implementations
//

NULLFS NTSTATUS DriverEntry(
    _In_ _Unreferenced_parameter_ PDRIVER_OBJECT driverObject,
    _In_ _Unreferenced_parameter_ PUNICODE_STRING registryPath
    )
{
    NTSTATUS status;
    UNICODE_STRING stringValue;

    UNREFERENCED_PARAMETER(registryPath);

    // Create the device object
    RtlInitUnicodeString(&stringValue, L"\\nullFS");
    status = IoCreateDevice(driverObject, 0, &stringValue, FILE_DEVICE_DISK_FILE_SYSTEM, 0, FALSE, &NullFsDeviceObject);
    if (!NT_SUCCESS(status))
    {
        FUNCTION_EXIT;
    }

    driverObject->DriverUnload = DriverUnload;

function_exit:

    if (!NT_SUCCESS(status))
    {
        DriverUnload(driverObject);
    }

    return status;
}

_Function_class_(DRIVER_UNLOAD) 
NULLFS void DriverUnload(
    _In_ _Unreferenced_parameter_ PDRIVER_OBJECT driverObject
    )
{
    UNREFERENCED_PARAMETER(driverObject);

    if (NullFsDeviceObject)
    {
        IoDeleteDevice(NullFsDeviceObject);
    }
}
