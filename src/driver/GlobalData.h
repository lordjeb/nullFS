#pragma once
#include <ntifs.h>

constexpr ULONG NfGlobalDataFlags_Resource_Initialized = 0x01;
constexpr ULONG NfGlobalDataFlags_Driver_Device_Created = 0x02;
constexpr ULONG NfGlobalDataFlags_Symbolic_Link_Created = 0x04;
constexpr ULONG NfGlobalDataFlags_File_System_Registered = 0x08;

// Holds all global data for the driver in a single structure
struct NfGlobalData
{
    ULONG                   flags;   // NfGlobalDataFlags_*
    ERESOURCE               lock;
    PDRIVER_OBJECT          driverObject;
    PDEVICE_OBJECT          fileSystemDeviceObject;
    FAST_IO_DISPATCH        fastIoDispatch;
    LIST_ENTRY              vcbList;
    CACHE_MANAGER_CALLBACKS cacheManagerCallbacks;

    struct
    {
        bool breakOnLoad;
        // NTSTATUS BreakOnNtStatus;
    } parameters;
};

extern NfGlobalData GlobalData;
