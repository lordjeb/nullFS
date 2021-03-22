#pragma once
#include <ntifs.h>

#define NF_GLOBAL_DATA_FLAGS_RESOURCE_INITIALIZED 0x01
#define NF_GLOBAL_DATA_FLAGS_DRIVER_DEVICE_CREATED 0x02
#define NF_GLOBAL_DATA_FLAGS_SYMBOLIC_LINK_CREATED 0x04
#define NF_GLOBAL_DATA_FLAGS_FILE_SYSTEM_REGISTERED 0x08

// Holds all global data for the driver in a single structure
typedef struct _NfGlobalData
{
    ULONG flags; // NF_GLOBAL_DATA_FLAGS_*
    ERESOURCE lock;
    PDRIVER_OBJECT driverObject;
    PDEVICE_OBJECT fileSystemDeviceObject;

    FAST_IO_DISPATCH FastIoDispatch;
    
    struct _Parameters
    {
        bool BreakOnLoad;
        NTSTATUS BreakOnNtStatus;
    } Parameters;
} NfGlobalData;

extern NfGlobalData globalData;
