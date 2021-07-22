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

    ERESOURCE lock;

    LIST_ENTRY nextVCB;

    ULONG flags;

    DEVICE_OBJECT* targetDeviceObject;   // Pointer to the actual physical/virtual device (or a top-level filter on that
                                         // device). Any IRPs we send should go to this.

    DEVICE_OBJECT* volumeDeviceObject;   // Pointer to the newly created device object

    VPB* vpb;

    FILE_OBJECT* streamFileObject;

    SECTION_OBJECT_POINTERS sectionObjectPointers;
};

struct NfVolumeDeviceObject
{
    DEVICE_OBJECT deviceObject;

    NfVolumeControlBlock vcb;
};

NfVolumeControlBlock* NfpInitializeVCB(NfVolumeControlBlock* vcb, VPB* vpb);
void NfUninitializeVCB(NfVolumeControlBlock* vcb);
