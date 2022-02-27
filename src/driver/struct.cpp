#include "pch.h"
#include "struct.h"
#include <nullFS/names.h>
#include "nodeType.h"

constexpr ULONG NfVolumeControlBlock_Initialized = 0x01;
constexpr ULONG NfVolumeControlBlock_Resource_Initialized = 0x02;

NfVolumeControlBlock* NfpInitializeVcb(NfVolumeControlBlock* vcb, VPB* vpb, DEVICE_OBJECT* targetDeviceObject)
{
    RtlZeroMemory(vcb, sizeof(NfVolumeControlBlock));

    vcb->volumeFileHeader.NodeTypeCode = NfNodeTypeCodeVolumeControlBlock;
    vcb->volumeFileHeader.NodeByteSize = sizeof(NfVolumeControlBlock);

    vcb->vpb = vpb;

    const auto rc = ExInitializeResourceLite(&(vcb->lock));
    ASSERT(NT_SUCCESS(rc));
    WI_SetFlag(vcb->flags, NfVolumeControlBlock_Resource_Initialized);

    vcb->targetDeviceObject = targetDeviceObject;   // TODO: Does this need a ObReferenceObject (FAT does it)

    // Create and initialize a stream file object for the volume
    vcb->virtualVolumeFile = IoCreateStreamFileObject(nullptr, vpb->RealDevice);
    ASSERT(vcb->virtualVolumeFile);
    NfTraceFsCtrl(WINEVENT_LEVEL_VERBOSE, "CreatedVcbVirtualVolumeFile",
                  TraceLoggingPointer(vcb->virtualVolumeFile, "FileObject"));
    vcb->virtualVolumeFile->FsContext = vcb;
    vcb->virtualVolumeFile->FsContext2 = nullptr;
    vcb->virtualVolumeFile->SectionObjectPointer = &vcb->sectionObjectPointers;
    vcb->virtualVolumeFile->Vpb = vpb;

    // Add this VCB to the global list of all VCBs
    ExAcquireResourceExclusiveLite(&(GlobalData.lock), TRUE);

    InsertTailList(&(GlobalData.vcbList), &vcb->vcbListEntry);

    CC_FILE_SIZES fileSizes{};
    fileSizes.AllocationSize.QuadPart = fileSizes.FileSize.QuadPart = sizeof(NfFirstSector);
    fileSizes.ValidDataLength.QuadPart = 0x7FFFFFFF'FFFFFFFF;
    CcInitializeCacheMap(vcb->virtualVolumeFile, &fileSizes, TRUE, &GlobalData.cacheManagerCallbacks, vcb);

    ExReleaseResourceLite(&(GlobalData.lock));

    ExInitializeFastMutex(&vcb->advancedFcbHeaderMutex);
    FsRtlSetupAdvancedHeader(&vcb->volumeFileHeader, &vcb->advancedFcbHeaderMutex);

    WI_SetFlag(vcb->flags, NfVolumeControlBlock_Initialized);

    return vcb;
}

void NfUninitializeVcb(NfVolumeControlBlock* vcb)
{
    ExAcquireResourceExclusiveLite(&(GlobalData.lock), TRUE);

    RemoveEntryList(&vcb->vcbListEntry);

    LARGE_INTEGER truncateSize{ 0 };
    CcUninitializeCacheMap(vcb->virtualVolumeFile, &truncateSize, nullptr);

    ExReleaseResourceLite(&GlobalData.lock);

    FsRtlTeardownPerStreamContexts(&vcb->volumeFileHeader);

    ObDereferenceObject(vcb->virtualVolumeFile);

    ExDeleteResourceLite(&vcb->lock);
}
