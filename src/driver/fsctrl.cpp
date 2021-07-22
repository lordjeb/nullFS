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

NTSTATUS NfMountVolume(DEVICE_OBJECT* targetDeviceObject, VPB* vpb)
{
    NTSTATUS rc{ STATUS_UNRECOGNIZED_VOLUME };
    NfVolumeDeviceObject* volumeDeviceObject{ nullptr };
    bool weClearedVerifyRequiredBit{ false };
    NfVolumeControlBlock* vcb{ nullptr };

    TRY
    {
        if (FlagOn(targetDeviceObject->Characteristics, FILE_REMOVABLE_MEDIA))
        {
            NfDbgPrint(DPFLTR_FS_CONTROL, "%s: Removable media\n", __FUNCTION__);

            // TODO: Make sure there is media in the drive?
        }

        // PARTITION_INFORMATION_EX partitionInformation;
        // IO_STATUS_BLOCK iosb = { 0 };
        // rc = NfpPerformDevIoCtrl(IOCTL_DISK_GET_PARTITION_INFO_EX, deviceObject, nullptr, 0, &partitionInformation,
        //                         sizeof(partitionInformation), FALSE, TRUE, nullptr);
        // TODO: What are we using the partition information for??? Fat uses it later to check for bad volumes)

        DISK_GEOMETRY geometry;
        rc = NfpPerformDevIoCtrl(IOCTL_DISK_GET_DRIVE_GEOMETRY, targetDeviceObject, nullptr, 0, &geometry,
                                 sizeof(geometry), FALSE, TRUE, nullptr);
        LEAVE_IF_NOT_SUCCESS(rc);

        rc = IoCreateDevice(globalData.driverObject, sizeof(NfVolumeDeviceObject) - sizeof(DEVICE_OBJECT), nullptr,
                            FILE_DEVICE_DISK_FILE_SYSTEM, 0, FALSE, (DEVICE_OBJECT**)&volumeDeviceObject);
        LEAVE_IF_NOT_SUCCESS(rc);

        NfDbgPrint(DPFLTR_FS_CONTROL, "%s: Created VDO [DevObj=%p]\n", __FUNCTION__, volumeDeviceObject);

        if (targetDeviceObject->AlignmentRequirement > volumeDeviceObject->deviceObject.AlignmentRequirement)
        {
            volumeDeviceObject->deviceObject.AlignmentRequirement = targetDeviceObject->AlignmentRequirement;
        }

        volumeDeviceObject->deviceObject.StackSize = targetDeviceObject->StackSize + 1;
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
            ExAllocatePoolWithTag(NonPagedPoolCacheAligned, sectorSize, TAG_REGISTRY_SECTOR_DATA)) };
        rc = NfpReadStorage(targetDeviceObject, 0, sectorSize, buffer.get());
        NfDbgPrint(DPFLTR_FS_CONTROL, "%s: NfpReadStorage %08x\n", __FUNCTION__, rc);
        LEAVE_IF_NOT_SUCCESS(rc);

        if (0xbeeff11e != buffer.get()->volumeSignature)
        {
            NfDbgPrint(DPFLTR_FS_CONTROL, "%s: Unrecognized file system signature\n", __FUNCTION__);
            LEAVE_WITH(rc = STATUS_UNRECOGNIZED_VOLUME);
        }

        NfDbgPrint(DPFLTR_FS_CONTROL, "%s: nullFS signature!\n", __FUNCTION__);

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
                NfDbgPrint(DPFLTR_FS_CONTROL, "%s: Delete VDO [DevObj=%p]\n", __FUNCTION__, volumeDeviceObject);
                IoDeleteDevice(&(volumeDeviceObject->deviceObject));
            }

            rc = STATUS_UNRECOGNIZED_VOLUME;
        }

        return rc;
    }
}

_Dispatch_type_(IRP_MJ_FILE_SYSTEM_CONTROL) _Function_class_(IRP_MJ_FILE_SYSTEM_CONTROL)
    _Function_class_(DRIVER_DISPATCH) extern "C" NTSTATUS
    NfFsdFileSystemControl(_In_ PDEVICE_OBJECT deviceObject, _Inout_ PIRP irp)
{
    PAGED_CODE();

    NTSTATUS rc{ STATUS_INVALID_DEVICE_REQUEST };
    TRY
    {
        PIO_STACK_LOCATION currentIrpStackLocation = IoGetCurrentIrpStackLocation(irp);

        NfDbgPrint(DPFLTR_FS_CONTROL, "%s: [DevObj=%p, IrpMn=%02x]\n", __FUNCTION__, deviceObject,
                   currentIrpStackLocation->MinorFunction);

        if (NfDeviceIsFileSystemDeviceObject(deviceObject))
        {
            NfDbgPrint(DPFLTR_FS_CONTROL, "%s: FileSystemDO\n", __FUNCTION__);

            switch (currentIrpStackLocation->MinorFunction)
            {
            case IRP_MN_USER_FS_REQUEST:
                NfDbgPrint(DPFLTR_FS_CONTROL, "%s: IRP_MN_USER_FS_REQUEST [Unhandled]\n", __FUNCTION__);
                break;

            case IRP_MN_MOUNT_VOLUME:
                NfDbgPrint(DPFLTR_FS_CONTROL, "%s: IRP_MN_MOUNT_VOLUME\n", __FUNCTION__);
                rc = NfMountVolume(currentIrpStackLocation->Parameters.MountVolume.DeviceObject,
                                   currentIrpStackLocation->Parameters.MountVolume.Vpb);
                break;

            default:
                NfDbgPrint(DPFLTR_FS_CONTROL, "%s: Unknown MinorFunction\n", __FUNCTION__);
                break;
            }

            LEAVE();
        }

        NfDbgPrint(DPFLTR_FS_CONTROL, "%s: Unrecognized device object\n", __FUNCTION__);
    }
    FINALLY
    {
        return NfCompleteRequest(irp, rc, 0);
    }
}
