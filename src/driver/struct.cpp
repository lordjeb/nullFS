#include "pch.h"
#include "struct.h"
#include <nullFS/names.h>

constexpr ULONG NfVolumeControlBlock_INITIALIZED = 0x01;
constexpr ULONG NfVolumeControlBlock_RESOURCE_INITIALIZED = 0x02;

NfVolumeControlBlock* NfpInitializeVcb(NfVolumeControlBlock* vcb, VPB* vpb, DEVICE_OBJECT* targetDeviceObject)
{
    RtlZeroMemory(vcb, sizeof(NfVolumeControlBlock));

    vcb->vpb = vpb;

    const auto rc = ExInitializeResourceLite(&(vcb->lock));
    ASSERT(NT_SUCCESS(rc));
    SetFlag(vcb->flags, NfVolumeControlBlock_RESOURCE_INITIALIZED);

    vcb->targetDeviceObject = targetDeviceObject;

    // Create and initialize a stream file object for the volume
    vcb->streamFileObject = IoCreateStreamFileObject(nullptr, vpb->RealDevice);
    ASSERT(vcb->streamFileObject);
    NfTraceFsCtrl(WINEVENT_LEVEL_VERBOSE, "CreatedVcbStreamFileObject",
                  TraceLoggingPointer(vcb->streamFileObject, "FileObject"));
    vcb->streamFileObject->FsContext = vcb;
    vcb->streamFileObject->FsContext2 = nullptr;
    vcb->streamFileObject->SectionObjectPointer = &(vcb->sectionObjectPointers);
    vcb->streamFileObject->Vpb = vpb;

    // Add this VCB to the global list of all VCBs
    ExAcquireResourceExclusiveLite(&(GlobalData.lock), TRUE);

    InsertTailList(&(GlobalData.nextVCB), &(vcb->nextVCB));

    CC_FILE_SIZES fileSizes{};
    fileSizes.AllocationSize.QuadPart = fileSizes.FileSize.QuadPart = sizeof(NfFirstSector);
    fileSizes.ValidDataLength.QuadPart = 0x7FFFFFFF'FFFFFFFF;
    CcInitializeCacheMap(vcb->streamFileObject, &(fileSizes), TRUE, &(GlobalData.cacheManagerCallbacks), vcb);

    ExReleaseResourceLite(&(GlobalData.lock));

    SetFlag(vcb->flags, NfVolumeControlBlock_INITIALIZED);

    return vcb;
}

void NfUninitializeVcb(NfVolumeControlBlock* vcb)
{
    ExAcquireResourceExclusiveLite(&(GlobalData.lock), TRUE);

    RemoveEntryList(&(vcb->nextVCB));

    LARGE_INTEGER truncateSize{ 0 };
    CcUninitializeCacheMap(vcb->streamFileObject, &truncateSize, nullptr);

    ExReleaseResourceLite(&(GlobalData.lock));

    ObDereferenceObject(vcb->streamFileObject);

    ExDeleteResourceLite(&(vcb->lock));
}
