#include "pch.h"
#include <nullFS/names.h>
#include "struct.h"
#include "dispatchRoutines.h"
#include "cacheManager.h"

using unique_registry_parameter_data =
    wil::unique_tagged_pool_ptr<KEY_VALUE_PARTIAL_INFORMATION*, TAG_REGISTRY_PARAMETER>;

// ---------------------------------------------------------------------------
// Driver unload functions
//

void NfUninitializeFileSystemDeviceObject()
{
    if (WI_IsFlagSet(GlobalData.flags, NfGlobalDataFlags_Symbolic_Link_Created))
    {
        UNICODE_STRING symbolicLinkName;
        WI_ClearFlag(GlobalData.flags, NfGlobalDataFlags_Symbolic_Link_Created);
        RtlInitUnicodeString(&symbolicLinkName, NF_DRIVER_SYMBOLIC_NAME);
        IoDeleteSymbolicLink(&symbolicLinkName);
    }

    if (WI_IsFlagSet(GlobalData.flags, NfGlobalDataFlags_Driver_Device_Created))
    {
        WI_ClearFlag(GlobalData.flags, NfGlobalDataFlags_Driver_Device_Created);
        IoDeleteDevice(GlobalData.fileSystemDeviceObject);
        GlobalData.fileSystemDeviceObject = nullptr;
    }
}

void NfUninitializeGlobals()
{
    if (WI_IsFlagSet(GlobalData.flags, NfGlobalDataFlags_Resource_Initialized))
    {
        WI_ClearFlag(GlobalData.flags, NfGlobalDataFlags_Resource_Initialized);
        ExDeleteResourceLite(&GlobalData.lock);
    }
}

_Function_class_(DRIVER_UNLOAD) void NfDriverUnload(_In_ PDRIVER_OBJECT driverObject)
{
    UNREFERENCED_PARAMETER(driverObject);

    NfTraceCommon(WINEVENT_LEVEL_INFO, "DriverUnload");

    NfUninitializeFileSystemDeviceObject();

    ExAcquireResourceExclusiveLite(&GlobalData.lock, TRUE);

    for (auto entry = GlobalData.vcbList.Flink; entry->Flink != &GlobalData.vcbList; entry = entry->Flink)
    {
        const auto vcb = CONTAINING_RECORD(entry, NfVolumeControlBlock, vcbListEntry);
        NfUninitializeVcb(vcb);
    }

    ExReleaseResourceLite(&GlobalData.lock);

    NfUninitializeGlobals();

    TraceLoggingUnregister(Logging::TraceLoggingProviderHandle);
}

// ---------------------------------------------------------------------------
// Driver entry functions
//

NTSTATUS NfInitializeFileSystemDeviceObject()
{
    ASSERT(GlobalData.driverObject);

    NTSTATUS rc{ STATUS_SUCCESS };
    TRY
    {
        UNICODE_STRING driverDeviceName = RTL_CONSTANT_STRING(NF_DRIVER_DEVICE_NAME);
        // TODO: Should be using FILE_DEVICE_SECURE_OPEN for security?
        rc = IoCreateDevice(GlobalData.driverObject, 0, &driverDeviceName, NF_DEVICE_TYPE, 0, FALSE,
                            &GlobalData.fileSystemDeviceObject);
        LEAVE_IF_NOT_SUCCESS(rc);
        WI_SetFlag(GlobalData.flags, NfGlobalDataFlags_Driver_Device_Created);

        UNICODE_STRING symbolicLinkName = RTL_CONSTANT_STRING(NF_DRIVER_SYMBOLIC_NAME);
        rc = IoCreateSymbolicLink(&symbolicLinkName, &driverDeviceName);
        LEAVE_IF_NOT_SUCCESS(rc);
        WI_SetFlag(GlobalData.flags, NfGlobalDataFlags_Symbolic_Link_Created);

        // Register our file system with the I/O subsystem (also adds a reference to the object)
        IoRegisterFileSystem(GlobalData.fileSystemDeviceObject);
        WI_SetFlag(GlobalData.flags, NfGlobalDataFlags_File_System_Registered);
    }
    FINALLY
    {
        return rc;
    }
}

void NfInitializeFsdDispatch()
{
    ASSERT(GlobalData.driverObject);

    // See p.396
#pragma warning(push)
#pragma warning(disable : 28175)   // Ok for file system driver
    GlobalData.driverObject->MajorFunction[IRP_MJ_CREATE] = static_cast<PDRIVER_DISPATCH>(NfFsdCreate);
    GlobalData.driverObject->MajorFunction[IRP_MJ_CLOSE] = static_cast<PDRIVER_DISPATCH>(NfFsdClose);
    GlobalData.driverObject->MajorFunction[IRP_MJ_CLEANUP] = static_cast<PDRIVER_DISPATCH>(NfFsdCleanup);
    GlobalData.driverObject->MajorFunction[IRP_MJ_FILE_SYSTEM_CONTROL] =
        static_cast<PDRIVER_DISPATCH>(NfFsdFileSystemControl);
    GlobalData.driverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = static_cast<PDRIVER_DISPATCH>(NfFsdDeviceControl);
    GlobalData.driverObject->MajorFunction[IRP_MJ_SHUTDOWN] = static_cast<PDRIVER_DISPATCH>(NfFsdShutdown);
    GlobalData.driverObject->MajorFunction[IRP_MJ_PNP] = static_cast<PDRIVER_DISPATCH>(NfFsdPnp);
    // GlobalData.driverObject->MajorFunction[IRP_MJ_READ] = static_cast<PDRIVER_DISPATCH>(NfFsdRead);
    // GlobalData.driverObject->MajorFunction[IRP_MJ_WRITE] = static_cast<PDRIVER_DISPATCH>(NfFsdWrite);
    // GlobalData.driverObject->MajorFunction[IRP_MJ_QUERY_INFORMATION] =
    // static_cast<PDRIVER_DISPATCH>(NfFsdQueryInformation);
    // GlobalData.driverObject->MajorFunction[IRP_MJ_SET_INFORMATION] =
    // static_cast<PDRIVER_DISPATCH>(NfFsdSetInformation); GlobalData.driverObject->MajorFunction[IRP_MJ_QUERY_EA] =
    // static_cast<PDRIVER_DISPATCH>(NfFsdQueryEa); GlobalData.driverObject->MajorFunction[IRP_MJ_SET_EA] =
    // static_cast<PDRIVER_DISPATCH>(NfFsdSetEa); GlobalData.driverObject->MajorFunction[IRP_MJ_FLUSH_BUFFERS] =
    // static_cast<PDRIVER_DISPATCH>(NfFsdFlushBuffers);
    // GlobalData.driverObject->MajorFunction[IRP_MJ_QUERY_VOLUME_INFORMATION] =
    // static_cast<PDRIVER_DISPATCH>(NfFsdQueryVolumeInformation);
    // GlobalData.driverObject->MajorFunction[IRP_MJ_SET_VOLUME_INFORMATION] =
    // static_cast<PDRIVER_DISPATCH>(NfFsdSetVolumeInformation);
    // GlobalData.driverObject->MajorFunction[IRP_MJ_DIRECTORY_CONTROL] =
    // static_cast<PDRIVER_DISPATCH>(NfFsdDirectoryControl);
    // GlobalData.driverObject->MajorFunction[IRP_MJ_LOCK_CONTROL] = static_cast<PDRIVER_DISPATCH>(NfFsdLockControl);

    const auto fastIoDispatch = GlobalData.driverObject->FastIoDispatch = &GlobalData.fastIoDispatch;
    fastIoDispatch->SizeOfFastIoDispatch = sizeof(FAST_IO_DISPATCH);
    // TODO: Initialize the FastIO stuff as well
#pragma warning(pop)
}

NTSTATUS NfInitializeGlobals(_In_ PDRIVER_OBJECT driverObject)
{
    NTSTATUS rc{ STATUS_SUCCESS };
    TRY
    {
        RtlZeroMemory(&GlobalData, sizeof(GlobalData));

        GlobalData.driverObject = driverObject;

        GlobalData.cacheManagerCallbacks.AcquireForLazyWrite = GlobalData.cacheManagerCallbacks.AcquireForReadAhead =
            NfCmAcquireNoOp;
        GlobalData.cacheManagerCallbacks.ReleaseFromLazyWrite = GlobalData.cacheManagerCallbacks.ReleaseFromReadAhead =
            NfCmReleaseNoOp;

        InitializeListHead(&(GlobalData.vcbList));

        rc = ExInitializeResourceLite(&GlobalData.lock);
        LEAVE_IF_NOT_SUCCESS(rc);
        WI_SetFlag(GlobalData.flags, NfGlobalDataFlags_Resource_Initialized);
    }
    FINALLY
    {
        return rc;
    }
}

NTSTATUS NfInitializeParameters(_In_ UNICODE_STRING* registryPath)
{
    NTSTATUS rc{ STATUS_SUCCESS };
    TRY
    {
        OBJECT_ATTRIBUTES         oaDriverKey = RTL_CONSTANT_OBJECT_ATTRIBUTES(registryPath, OBJ_CASE_INSENSITIVE);
        wil::unique_kernel_handle driverKey;
        rc = ZwOpenKey(driverKey.addressof(), KEY_READ, &oaDriverKey);
        if (STATUS_SUCCESS == rc)
        {
            UNICODE_STRING            parametersKeyName = RTL_CONSTANT_STRING(L"parameters");
            OBJECT_ATTRIBUTES         oaParametersKey{ sizeof(OBJECT_ATTRIBUTES),
                                               driverKey.get(),
                                               RTL_CONST_CAST(PUNICODE_STRING)(&parametersKeyName),
                                               OBJ_CASE_INSENSITIVE,
                                               NULL,
                                               NULL };
            wil::unique_kernel_handle parametersKey;
            rc = ZwOpenKey(parametersKey.addressof(), KEY_READ, &oaParametersKey);
            if (STATUS_SUCCESS == rc)
            {
                const ULONG cbKvpi = FIELD_OFFSET(KEY_VALUE_PARTIAL_INFORMATION, Data) + sizeof(ULONG);
                unique_registry_parameter_data kvpi{ static_cast<KEY_VALUE_PARTIAL_INFORMATION*>(
                    ExAllocatePool2(POOL_FLAG_PAGED, cbKvpi, TAG_REGISTRY_PARAMETER)) };
                if (!kvpi.is_valid())
                {
                    LEAVE_WITH(rc = STATUS_INSUFFICIENT_RESOURCES);
                }

                UNICODE_STRING breakOnLoadValueName = RTL_CONSTANT_STRING(L"breakOnLoad");
                ULONG          returnLength;
                rc = ZwQueryValueKey(parametersKey.get(), &breakOnLoadValueName, KeyValuePartialInformation, kvpi.get(),
                                     cbKvpi, &returnLength);
                if (STATUS_SUCCESS == rc && kvpi.get()->Type == REG_DWORD && kvpi.get()->DataLength == sizeof(ULONG))
                {
                    GlobalData.parameters.breakOnLoad = (0 != *((PULONG)&kvpi.get()->Data[0]));
                }

                // UNICODE_STRING breakOnNtStatusValueName = RTL_CONSTANT_STRING(L"BreakOnNtStatus");
                // rc = ZwQueryValueKey(parametersKey.get(), &breakOnNtStatusValueName, KeyValuePartialInformation,
                //                      kvpi.get(), cbKvpi, &returnLength);
                // if (STATUS_SUCCESS == rc && kvpi.get()->Type == REG_DWORD && kvpi.get()->DataLength == sizeof(ULONG))
                //{
                //     globalData.parameters.BreakOnNtStatus = *((PULONG)&kvpi.get()->Data[0]);
                // }
            }
        }

        rc = STATUS_SUCCESS;
    }
    FINALLY
    {
        return rc;
    }
}

extern "C" DRIVER_INITIALIZE DriverEntry;

extern "C" NTSTATUS DriverEntry(_In_ DRIVER_OBJECT* driverObject, _In_ UNICODE_STRING* registryPath)
{
    NTSTATUS rc{ STATUS_SUCCESS };
    TRY
    {
        UNREFERENCED_PARAMETER(registryPath);

        TraceLoggingRegister(Logging::TraceLoggingProviderHandle);

        NfTraceCommon(WINEVENT_LEVEL_INFO, "DriverEntry", TraceLoggingString(__TIMESTAMP__, "BuildTimestamp"));

        rc = NfInitializeGlobals(driverObject);
        LEAVE_IF_NOT_SUCCESS(rc);

        rc = NfInitializeParameters(registryPath);
        LEAVE_IF_NOT_SUCCESS(rc);

        if (GlobalData.parameters.breakOnLoad)
        {
            DbgBreakPoint();
        }

        // Initialize the driver object,
        NfInitializeFsdDispatch();
        // Allow unload for debugging purposes only
#if defined(DBG)
        driverObject->DriverUnload = NfDriverUnload;
#endif

        // Create the device object
        rc = NfInitializeFileSystemDeviceObject();
        LEAVE_IF_NOT_SUCCESS(rc);
    }
    FINALLY
    {
        if (!NT_SUCCESS(rc))
        {
            NfTraceCommon(WINEVENT_LEVEL_ERROR, "DriverEntryFailed");

            NfDriverUnload(driverObject);
        }

        return rc;
    }
}
