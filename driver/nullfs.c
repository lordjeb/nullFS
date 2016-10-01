#include "common.h"
#include <ntifs.h>
//#include <dontuse.h>
//#include <suppress.h>

#pragma prefast(disable:__WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode")


// ---------------------------------------------------------------------------
// Global variables
//

PDEVICE_OBJECT NullFsDeviceObject; // Control Device Object (CDO)

// ---------------------------------------------------------------------------
// Function prototypes
//

DRIVER_INITIALIZE DriverEntry;

NTSTATUS DriverEntry(
    _In_ PDRIVER_OBJECT driverObject,
    _In_ _Unreferenced_parameter_ PUNICODE_STRING registryPath
    );

_Function_class_(DRIVER_UNLOAD)
void DriverUnload(
    _In_ _Unreferenced_parameter_ PDRIVER_OBJECT driverObject
    );

void InitializeFsdDispatch(_In_ PDRIVER_OBJECT driverObject);

// ---------------------------------------------------------------------------
// Assign text sections for each routine.
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, DriverUnload)
#pragma alloc_text(INIT, InitializeFsdDispatch)
#endif


// ---------------------------------------------------------------------------
// Function implementations
//

NTSTATUS DriverEntry(
    _In_ PDRIVER_OBJECT driverObject,
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

	// Initialize the device object
    driverObject->DriverUnload = DriverUnload;
	InitializeFsdDispatch(driverObject);

	// Register our file system with the I/O subsystem
	IoRegisterFileSystem(NullFsDeviceObject);
	ObReferenceObject(NullFsDeviceObject);

function_exit:

    if (!NT_SUCCESS(status))
    {
		IoDeleteDevice(NullFsDeviceObject);
    }

    return status;
}

_Function_class_(DRIVER_UNLOAD) 
void DriverUnload(
    _In_ _Unreferenced_parameter_ PDRIVER_OBJECT driverObject
    )
{
    UNREFERENCED_PARAMETER(driverObject);

	// Note: The FastFat example does not call IoUnregisterFileSystem. We probably need one or the other of the following.
	//IoUnregisterFileSystem(NullFsDeviceObject);
	ObDereferenceObject(NullFsDeviceObject);
}

void InitializeFsdDispatch(_In_ PDRIVER_OBJECT driverObject)
{
	driverObject->MajorFunction[IRP_MJ_CREATE] = (PDRIVER_DISPATCH)FsdCreate;
	driverObject->MajorFunction[IRP_MJ_CLOSE] = (PDRIVER_DISPATCH)FsdClose;
	//driverObject->MajorFunction[IRP_MJ_READ] = (PDRIVER_DISPATCH)FatFsdRead;
	//driverObject->MajorFunction[IRP_MJ_WRITE] = (PDRIVER_DISPATCH)FatFsdWrite;
	//driverObject->MajorFunction[IRP_MJ_QUERY_INFORMATION] = (PDRIVER_DISPATCH)FatFsdQueryInformation;
	//driverObject->MajorFunction[IRP_MJ_SET_INFORMATION] = (PDRIVER_DISPATCH)FatFsdSetInformation;
	//driverObject->MajorFunction[IRP_MJ_QUERY_EA] = (PDRIVER_DISPATCH)FatFsdQueryEa;
	//driverObject->MajorFunction[IRP_MJ_SET_EA] = (PDRIVER_DISPATCH)FatFsdSetEa;
	//driverObject->MajorFunction[IRP_MJ_FLUSH_BUFFERS] = (PDRIVER_DISPATCH)FatFsdFlushBuffers;
	//driverObject->MajorFunction[IRP_MJ_QUERY_VOLUME_INFORMATION] = (PDRIVER_DISPATCH)FatFsdQueryVolumeInformation;
	//driverObject->MajorFunction[IRP_MJ_SET_VOLUME_INFORMATION] = (PDRIVER_DISPATCH)FatFsdSetVolumeInformation;
	driverObject->MajorFunction[IRP_MJ_CLEANUP] = (PDRIVER_DISPATCH)FsdCleanup;
	//driverObject->MajorFunction[IRP_MJ_DIRECTORY_CONTROL] = (PDRIVER_DISPATCH)FatFsdDirectoryControl;
	//driverObject->MajorFunction[IRP_MJ_FILE_SYSTEM_CONTROL] = (PDRIVER_DISPATCH)FatFsdFileSystemControl;
	//driverObject->MajorFunction[IRP_MJ_LOCK_CONTROL] = (PDRIVER_DISPATCH)FatFsdLockControl;
	//driverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = (PDRIVER_DISPATCH)FatFsdDeviceControl;
	//driverObject->MajorFunction[IRP_MJ_SHUTDOWN] = (PDRIVER_DISPATCH)FatFsdShutdown;
	//driverObject->MajorFunction[IRP_MJ_PNP] = (PDRIVER_DISPATCH)FatFsdPnp;
}
