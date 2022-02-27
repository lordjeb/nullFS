#pragma once
#include <ntifs.h>

struct NfVolumeControlBlock
{
    // TODO: Add (and initialize a node type and node size)
    // ExResource for synchronization
    // List of FCBs
    // List of Notify IRPs
    // List of volume opens?
    // Notify IRP list mutex
    // Filesize and allocation size?
    // Valid data length

    FSRTL_ADVANCED_FCB_HEADER volumeFileHeader;
    FAST_MUTEX                advancedFcbHeaderMutex;
    ERESOURCE                 lock;
    LIST_ENTRY                vcbListEntry;
    ULONG                     flags;
    DEVICE_OBJECT*            targetDeviceObject;
    VPB*                      vpb;
    FILE_OBJECT*              virtualVolumeFile;
    SECTION_OBJECT_POINTERS   sectionObjectPointers;
};

struct NfVolumeDeviceObject
{
    DEVICE_OBJECT        deviceObject;
    NfVolumeControlBlock vcb;
};

NfVolumeControlBlock* NfpInitializeVcb(NfVolumeControlBlock* vcb, VPB* vpb, DEVICE_OBJECT* targetDeviceObject);

void NfUninitializeVcb(NfVolumeControlBlock* vcb);
