#include "pch.h"
#include "struct.h"
#include <nullFS/names.h>

constexpr ULONG NfVolumeControlBlock_INITIALIZED = 0x01;
constexpr ULONG NfVolumeControlBlock_RESOURCE_INITIALIZED = 0x02;

NfVolumeControlBlock* NfpInitializeVCB(NfVolumeControlBlock* vcb, VPB* vpb)
{
    RtlZeroMemory(vcb, sizeof(NfVolumeControlBlock));

    vcb->vpb = vpb;

    auto rc = ExInitializeResourceLite(&(vcb->lock));
    ASSERT(NT_SUCCESS(rc));
    SetFlag(vcb->flags, NfVolumeControlBlock_RESOURCE_INITIALIZED);

    // Create and initialize a stream file object for the volume
    vcb->streamFileObject = IoCreateStreamFileObject(nullptr, vpb->RealDevice);
    ASSERT(vcb->streamFileObject);
    NfDbgPrint(DPFLTR_FS_CONTROL, "%s: Created stream file object [FileObj=%p]\n", __FUNCTION__, vcb->streamFileObject);
    vcb->streamFileObject->FsContext = vcb;
    vcb->streamFileObject->FsContext2 = nullptr;
    vcb->streamFileObject->SectionObjectPointer = &(vcb->sectionObjectPointers);
    vcb->streamFileObject->Vpb = vpb;

    // Add this VCB to the global list of all VCBs
    ExAcquireResourceExclusiveLite(&(globalData.lock), TRUE);

    InsertTailList(&(globalData.nextVCB), &(vcb->nextVCB));

    CC_FILE_SIZES fileSizes{};
    fileSizes.AllocationSize.QuadPart = fileSizes.FileSize.QuadPart = sizeof(NfFirstSector);
    fileSizes.ValidDataLength.QuadPart = 0x7FFFFFFF'FFFFFFFF;
    CcInitializeCacheMap(vcb->streamFileObject, &(fileSizes), TRUE, &(globalData.cacheManagerCallbacks), vcb);

    ExReleaseResourceLite(&(globalData.lock));

    SetFlag(vcb->flags, NfVolumeControlBlock_INITIALIZED);

    return vcb;
}

void NfUninitializeVCB(NfVolumeControlBlock* vcb)
{
    ExAcquireResourceExclusiveLite(&(globalData.lock), TRUE);

    RemoveEntryList(&(vcb->nextVCB));

    LARGE_INTEGER truncateSize{ 0 };
    CcUninitializeCacheMap(vcb->streamFileObject, &truncateSize, nullptr);

    ExReleaseResourceLite(&(globalData.lock));

    ObDereferenceObject(vcb->streamFileObject);

    ExDeleteResourceLite(&(vcb->lock));
}
