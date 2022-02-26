#pragma once
#include <ntifs.h>

constexpr ULONG NF_GLOBAL_DATA_FLAGS_RESOURCE_INITIALIZED = 0x01;
constexpr ULONG NF_GLOBAL_DATA_FLAGS_DRIVER_DEVICE_CREATED = 0x02;
constexpr ULONG NF_GLOBAL_DATA_FLAGS_SYMBOLIC_LINK_CREATED = 0x04;
constexpr ULONG NF_GLOBAL_DATA_FLAGS_FILE_SYSTEM_REGISTERED = 0x08;

// Holds all global data for the driver in a single structure
struct NfGlobalData
{
    ULONG flags;   // NF_GLOBAL_DATA_FLAGS_*
    ERESOURCE lock;
    PDRIVER_OBJECT driverObject;
    PDEVICE_OBJECT fileSystemDeviceObject;

    FAST_IO_DISPATCH fastIoDispatch;
    LIST_ENTRY nextVCB;
    CACHE_MANAGER_CALLBACKS cacheManagerCallbacks;

    struct _Parameters
    {
        bool BreakOnLoad;
        //NTSTATUS BreakOnNtStatus;
    } Parameters;
};

extern NfGlobalData GlobalData;
