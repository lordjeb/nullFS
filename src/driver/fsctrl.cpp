#include "pch.h"
#include "flowControl.h"
#include "dispatchRoutines.h"
#include "struct.h"
#include <nullFS/names.h>

// ---------------------------------------------------------------------------
// Function implementations
//

NTSTATUS
NfpPerformDevIoCtrl(_In_ ULONG ioControlCode, _In_ PDEVICE_OBJECT device, _In_opt_ PVOID inputBuffer,
                    _In_ ULONG inputBufferLength, _Out_opt_ PVOID outputBuffer, _In_ ULONG outputBufferLength,
                    _In_ BOOLEAN internalDeviceIoControl, _In_ BOOLEAN overrideVerify, _Out_opt_ PIO_STATUS_BLOCK iosb)
{
    NTSTATUS rc;
    IO_STATUS_BLOCK LocalIosb{};
    PIO_STATUS_BLOCK IosbToUse = &LocalIosb;

    if (iosb != nullptr)
    {
        IosbToUse = iosb;
    }

    IosbToUse->Status = 0;
    IosbToUse->Information = 0;

    KEVENT Event;
    KeInitializeEvent(&Event, NotificationEvent, FALSE);

    IRP* irp = IoBuildDeviceIoControlRequest(ioControlCode, device, inputBuffer, inputBufferLength, outputBuffer,
                                             outputBufferLength, internalDeviceIoControl, &Event, IosbToUse);
    if (irp == nullptr)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    if (overrideVerify)
    {
        SetFlag(IoGetNextIrpStackLocation(irp)->Flags, SL_OVERRIDE_VERIFY_VOLUME);
    }

    rc = IoCallDriver(device, irp);

    if (STATUS_PENDING == rc)
    {
        KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, nullptr);
        rc = IosbToUse->Status;
    }

    return rc;
}

NTSTATUS NfpReadStorage(DEVICE_OBJECT* deviceObject, long long offset, unsigned long sectorSize, void* buffer)
{
    LARGE_INTEGER off{};
    off.QuadPart = offset;

    IO_STATUS_BLOCK iosb{};

    KEVENT event;
    KeInitializeEvent(&event, NotificationEvent, FALSE);

    PIRP irp = IoBuildSynchronousFsdRequest(IRP_MJ_READ, deviceObject, buffer, sectorSize, &off, &event, &iosb);
    if (!irp)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    auto rc = IoCallDriver(deviceObject, irp);
    if (rc == STATUS_PENDING)
    {
        KeWaitForSingleObject(&event, Suspended, KernelMode, FALSE, NULL);
        rc = iosb.Status;
    }

    return rc;
}

NTSTATUS NfMountVolume(PIO_STACK_LOCATION irpSp)
{
    NTSTATUS rc{ STATUS_UNRECOGNIZED_VOLUME };
    DEVICE_OBJECT* targetDeviceObject{ irpSp->Parameters.MountVolume.DeviceObject };
    VPB* vpb{ irpSp->Parameters.MountVolume.Vpb };
    NfVolumeDeviceObject* volumeDeviceObject{ nullptr };
    bool weClearedVerifyRequiredBit{ false };
    NfVolumeControlBlock* vcb{ nullptr };

    TRY
    {
        if (FlagOn(targetDeviceObject->Characteristics, FILE_REMOVABLE_MEDIA))
        {
            // TODO: Make sure there is media in the drive?
        }

        // PARTITION_INFORMATION_EX partitionInformation;
        // IO_STATUS_BLOCK iosb = { 0 };
        // rc = NfpPerformDevIoCtrl(IOCTL_DISK_GET_PARTITION_INFO_EX, deviceObject, nullptr, 0,
        // &partitionInformation,
        //                         sizeof(partitionInformation), FALSE, TRUE, nullptr);
        // TODO: What are we using the partition information for??? Fat uses it later to check for bad volumes)

        DISK_GEOMETRY geometry;
        rc = NfpPerformDevIoCtrl(IOCTL_DISK_GET_DRIVE_GEOMETRY, targetDeviceObject, nullptr, 0, &geometry,
                                 sizeof(geometry), FALSE, TRUE, nullptr);
        LEAVE_IF_NOT_SUCCESS(rc);

        rc = IoCreateDevice(globalData.driverObject, sizeof(NfVolumeDeviceObject) - sizeof(DEVICE_OBJECT), nullptr,
                            FILE_DEVICE_DISK_FILE_SYSTEM, 0, FALSE, (DEVICE_OBJECT**)&volumeDeviceObject);
        LEAVE_IF_NOT_SUCCESS(rc);

        NfTraceFsCtrl(WINEVENT_LEVEL_VERBOSE, "VdoCreated", TraceLoggingPointer(volumeDeviceObject));

        if (targetDeviceObject->AlignmentRequirement > volumeDeviceObject->deviceObject.AlignmentRequirement)
        {
            volumeDeviceObject->deviceObject.AlignmentRequirement = targetDeviceObject->AlignmentRequirement;
        }

        volumeDeviceObject->deviceObject.StackSize = targetDeviceObject->StackSize + 1;
#pragma warning(suppress : 28175)   // Ok for file system driver
        volumeDeviceObject->deviceObject.SectorSize = (USHORT)geometry.BytesPerSector;

        // Indicate that the device object is fully initialized
        ClearFlag(volumeDeviceObject->deviceObject.Flags, DO_DEVICE_INITIALIZING);

        // Required for creating the streamFileObject for the volume in NfpInitializeVCB
        vpb->DeviceObject = reinterpret_cast<DEVICE_OBJECT*>(volumeDeviceObject);

        // Clear DO_VERIFY_VOLUME so we can do reads
        if (FlagOn(vpb->RealDevice->Flags, DO_VERIFY_VOLUME))
        {
            ClearFlag(vpb->RealDevice->Flags, DO_VERIFY_VOLUME);
            weClearedVerifyRequiredBit = true;
        }

        NfpInitializeVCB(&volumeDeviceObject->vcb, vpb);
        vcb = &(volumeDeviceObject->vcb);

        auto sectorSize = geometry.BytesPerSector;
        wil::unique_tagged_pool_ptr<NfFirstSector*, TAG_REGISTRY_SECTOR_DATA> buffer{ static_cast<NfFirstSector*>(
            ExAllocatePool2(POOL_FLAG_NON_PAGED | POOL_FLAG_CACHE_ALIGNED, sectorSize, TAG_REGISTRY_SECTOR_DATA)) };
        rc = NfpReadStorage(targetDeviceObject, 0, sectorSize, buffer.get());
        TRACE_AND_LEAVE_IF_NOT_SUCCESS(rc, "VolumeMountFailedToReadStorage", Logging::Keyword::FsCtrl);

        if (0xbeeff11e != buffer.get()->volumeSignature)
        {
            NfTraceFsCtrl(WINEVENT_LEVEL_VERBOSE, "UnrecognizedVolumeSignature");
            LEAVE_WITH(rc = STATUS_UNRECOGNIZED_VOLUME);
        }

        NfTraceFsCtrl(WINEVENT_LEVEL_VERBOSE, "VolumeMounted");

        rc = STATUS_SUCCESS;
    }
    FINALLY
    {
        if (weClearedVerifyRequiredBit)
        {
            SetFlag(vpb->RealDevice->Flags, DO_VERIFY_VOLUME);
        }

        if (!NT_SUCCESS(rc))
        {
            if (vcb != nullptr)
            {
                NfUninitializeVCB(vcb);
            }

            if (volumeDeviceObject != nullptr)
            {
                // VCB was never initialized, so we need to just delete the device object
                IoDeleteDevice(&(volumeDeviceObject->deviceObject));

                NfTraceFsCtrl(WINEVENT_LEVEL_VERBOSE, "VdoDeleted",
                              TraceLoggingPointer(&(volumeDeviceObject->deviceObject), "volumeDeviceObject"));
            }

            rc = STATUS_UNRECOGNIZED_VOLUME;
        }

        return rc;
    }
}

NTSTATUS NfUserFsCtrl(PIO_STACK_LOCATION irpSp)
{
    switch (irpSp->Parameters.FileSystemControl.FsControlCode)
    {
    case FSCTL_LOCK_VOLUME:
        NfTraceFsCtrl(WINEVENT_LEVEL_VERBOSE, "LockVolume");
        break;

    case FSCTL_UNLOCK_VOLUME:
        NfTraceFsCtrl(WINEVENT_LEVEL_VERBOSE, "UnlockVolume");
        break;

    case FSCTL_DISMOUNT_VOLUME:
        NfTraceFsCtrl(WINEVENT_LEVEL_VERBOSE, "DismountVolume");
        break;

    default:
        NfTraceFsCtrl(WINEVENT_LEVEL_VERBOSE, "UnhandledFsControlCode",
                      TraceLoggingULong(irpSp->Parameters.FileSystemControl.FsControlCode, "FsControlCode"));
        break;
    }

    return STATUS_NOT_IMPLEMENTED;
}

_Dispatch_type_(IRP_MJ_FILE_SYSTEM_CONTROL) _Function_class_(IRP_MJ_FILE_SYSTEM_CONTROL)
    _Function_class_(DRIVER_DISPATCH) extern "C" NTSTATUS
    NfFsdFileSystemControl(_In_ PDEVICE_OBJECT deviceObject, _Inout_ PIRP irp)
{
    NTSTATUS rc{ STATUS_NOT_IMPLEMENTED };
    TRY
    {
        auto irpSp = IoGetCurrentIrpStackLocation(irp);

        switch (irpSp->MinorFunction)
        {
        case IRP_MN_USER_FS_REQUEST:
            NfTraceFsCtrl(WINEVENT_LEVEL_VERBOSE, "UserFsRequest", TraceLoggingPointer(deviceObject));

            rc = NfUserFsCtrl(irpSp);
            break;

        case IRP_MN_MOUNT_VOLUME:
            NfTraceFsCtrl(WINEVENT_LEVEL_VERBOSE, "MountVolume", TraceLoggingPointer(deviceObject),
                          TraceLoggingPointer(irpSp->Parameters.MountVolume.DeviceObject, "mountingDeviceObject"),
                          TraceLoggingPointer(irpSp->Parameters.MountVolume.Vpb, "vpb"));

            rc = NfMountVolume(irpSp);
            break;

        case IRP_MN_VERIFY_VOLUME:
            NfTraceFsCtrl(WINEVENT_LEVEL_VERBOSE, "VerifyVolume", TraceLoggingPointer(deviceObject));
            break;

        default:
            NfTraceFsCtrl(WINEVENT_LEVEL_VERBOSE, "UnhandledMinorFunction", TraceLoggingPointer(deviceObject),
                          TraceLoggingUInt8(irpSp->MinorFunction, "MinorFunction"));
            break;
        }
    }
    FINALLY
    {
        return NfCompleteRequest(irp, rc, 0);
    }
}
