#include "pch.h"
#include "VirtualDisk.h"
#include <virtdisk.h>
#include <wil/resource.h>
#include <conio.h>

typedef wil::unique_any_handle_invalid<decltype(&::FindVolumeClose), ::FindVolumeClose> unique_hfindvolume;

#if !defined(PARTITION_BASIC_DATA_GUID)
const GUID PARTITION_BASIC_DATA_GUID = {
    0xebd0a0a2, 0xb9e5, 0x4433, { 0x87, 0xc0, 0x68, 0xb6, 0xb7, 0x26, 0x99, 0xc7 }
};
#endif

VirtualDisk::VirtualDisk(const std::wstring& vhdFilename) : vhdFilename_{ vhdFilename }
{
}

VirtualDisk::~VirtualDisk()
{
}

void VirtualDisk::AssignDriveLetter(const std::wstring& driveLetter)
{
    if (!vhd_.is_valid())
    {
        THROW_WIN32(ERROR_INVALID_PARAMETER);
    }

    auto volumeName = WaitForVolumeName(3000) + L'\\';

    auto driveLetterWithBackslash = driveLetter + L'\\';
    THROW_IF_WIN32_BOOL_FALSE(SetVolumeMountPoint(driveLetterWithBackslash.c_str(), volumeName.c_str()));
}

void VirtualDisk::Attach()
{
    ATTACH_VIRTUAL_DISK_PARAMETERS attachParameters;
    memset(&attachParameters, 0, sizeof(attachParameters));
    attachParameters.Version = ATTACH_VIRTUAL_DISK_VERSION_1;

    THROW_IF_WIN32_ERROR(AttachVirtualDisk(vhd_.get(), nullptr, ATTACH_VIRTUAL_DISK_FLAG_NO_DRIVE_LETTER, 0,
                                           &attachParameters, nullptr));
}

void VirtualDisk::Create(ULONGLONG diskSizeInBytes)
{
    if (vhd_.is_valid())
    {
        THROW_WIN32(ERROR_INVALID_PARAMETER);
    }

    VIRTUAL_STORAGE_TYPE storageType;
    storageType.DeviceId = VIRTUAL_STORAGE_TYPE_DEVICE_UNKNOWN;
    storageType.VendorId = VIRTUAL_STORAGE_TYPE_VENDOR_UNKNOWN;

    CREATE_VIRTUAL_DISK_PARAMETERS createParameters;
    memset(&createParameters, 0, sizeof(createParameters));
    createParameters.Version = CREATE_VIRTUAL_DISK_VERSION_2;
    THROW_IF_WIN32_ERROR(UuidCreate(&createParameters.Version2.UniqueId));
    createParameters.Version2.MaximumSize = diskSizeInBytes;
    createParameters.Version2.BlockSizeInBytes = 0;
    createParameters.Version2.SectorSizeInBytes = 512;
    createParameters.Version2.PhysicalSectorSizeInBytes = 512;

    THROW_IF_WIN32_ERROR(CreateVirtualDisk(&storageType, vhdFilename_.c_str(), VIRTUAL_DISK_ACCESS_NONE, nullptr,
                                           CREATE_VIRTUAL_DISK_FLAG_NONE, 0, &createParameters, nullptr,
                                           vhd_.addressof()));

    Attach();

    Initialize();
}

void VirtualDisk::Delete()
{
    vhd_.reset();
    DeleteFile(vhdFilename_.c_str());
}

void VirtualDisk::Detach()
{
    if (!vhd_.is_valid())
    {
        THROW_WIN32(ERROR_INVALID_PARAMETER);
    }

    THROW_IF_WIN32_ERROR(DetachVirtualDisk(vhd_.get(), DETACH_VIRTUAL_DISK_FLAG_NONE, 0));
}

std::wstring VirtualDisk::GetVolumeName()
{
    wchar_t physicalPath[MAX_PATH];
    ULONG physicalPathLen = _countof(physicalPath);
    THROW_IF_WIN32_ERROR(GetVirtualDiskPhysicalPath(vhd_.get(), &physicalPathLen, physicalPath));

    auto deviceNumberOfPhysicalDisk = _wtol(physicalPath + wcslen(LR"(\\.\PHYSICALDRIVE)"));

    bool found{ false };
    wchar_t volumeName[MAX_PATH];
    unique_hfindvolume find{ FindFirstVolume(volumeName, _countof(volumeName)) };
    if (find.is_valid())
    {
        do
        {
            // Remove final backslash so we are opening the volume rather than the root directory on the volume
            auto pch = volumeName + wcslen(volumeName) - 1;
            *pch = L'\0';

            wil::unique_hfile volume{ CreateFile(volumeName,
                                                 FILE_READ_ACCESS | FILE_WRITE_ACCESS | FILE_WRITE_ATTRIBUTES,
                                                 FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
                                                 FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, 0) };
            THROW_LAST_ERROR_IF(!volume.is_valid());

            DWORD bytesReturned;
            STORAGE_DEVICE_NUMBER sdn;
            THROW_IF_WIN32_BOOL_FALSE(DeviceIoControl(volume.get(), IOCTL_STORAGE_GET_DEVICE_NUMBER, nullptr, 0, &sdn,
                                                      sizeof(sdn), &bytesReturned, nullptr));
            if (sdn.DeviceNumber == deviceNumberOfPhysicalDisk)
            {
                found = true;
                break;
            }
        } while (FindNextVolume(find.get(), volumeName, _countof(volumeName)));
    }

    auto gle = GetLastError();
    if (ERROR_SUCCESS != gle && ERROR_NO_MORE_FILES != gle)
    {
        THROW_WIN32(gle);
    }

    if (!found)
    {
        THROW_WIN32(ERROR_FILE_NOT_FOUND);
    }

    return volumeName;
}

void VirtualDisk::Initialize()
{
    wchar_t physicalPath[MAX_PATH];
    ULONG physicalPathLen = _countof(physicalPath);
    THROW_IF_WIN32_ERROR(GetVirtualDiskPhysicalPath(vhd_.get(), &physicalPathLen, physicalPath));

    wil::unique_hfile physicalDrive{ CreateFile(physicalPath, FILE_ALL_ACCESS, 0, nullptr, OPEN_EXISTING,
                                                FILE_ATTRIBUTE_NORMAL, nullptr) };
    THROW_LAST_ERROR_IF(!physicalDrive.is_valid());

    DWORD bytesReturned;

    // Create the disk partition table (GPT)
    //
    CREATE_DISK createDisk;
    memset(&createDisk, 0, sizeof(createDisk));
    createDisk.PartitionStyle = PARTITION_STYLE_GPT;
    THROW_IF_WIN32_ERROR(UuidCreate(&createDisk.Gpt.DiskId));
    createDisk.Gpt.MaxPartitionCount = 128;
    THROW_IF_WIN32_BOOL_FALSE(DeviceIoControl(physicalDrive.get(), IOCTL_DISK_CREATE_DISK, (LPVOID)&createDisk,
                                              sizeof(createDisk), nullptr, 0, &bytesReturned, nullptr));

    // TODO: Create a window handle, and use RegisterDeviceNotification to wait for this (set up beforehand calling IOCTL?)
    Sleep(2000);

    // Create a partition in the disk
    //
    DISK_GEOMETRY_EX diskGeometry;
    THROW_IF_WIN32_BOOL_FALSE(DeviceIoControl(physicalDrive.get(), IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, nullptr, 0,
                                              &diskGeometry, sizeof(DISK_GEOMETRY_EX), &bytesReturned, nullptr));

    // Note: a GPT disk has 34 reserved sectors at the beginning and 33 at the end (wikipedia)
    auto gptPrimaryHeaderReserved = 34 * diskGeometry.Geometry.BytesPerSector;
    auto gptSecondaryHeaderReserved = 33 * diskGeometry.Geometry.BytesPerSector;
    auto offset = 2 * 1024 * diskGeometry.Geometry.BytesPerSector;
    // TODO: Where did this number come from and why different than gptPrimaryHeaderReserved???
    auto partitionUsableSize = diskGeometry.DiskSize.QuadPart - offset - gptSecondaryHeaderReserved;

    DRIVE_LAYOUT_INFORMATION_EX driveLayoutInfo;
    memset(&driveLayoutInfo, 0, sizeof(driveLayoutInfo));
    driveLayoutInfo.PartitionStyle = PARTITION_STYLE_GPT;
    driveLayoutInfo.Gpt.StartingUsableOffset.QuadPart = gptPrimaryHeaderReserved;
    driveLayoutInfo.Gpt.UsableLength.QuadPart = partitionUsableSize;
    driveLayoutInfo.Gpt.MaxPartitionCount = 128;
    driveLayoutInfo.Gpt.DiskId = createDisk.Gpt.DiskId;
    driveLayoutInfo.PartitionCount = 1;
    driveLayoutInfo.PartitionEntry[0].PartitionStyle = PARTITION_STYLE_GPT;
    driveLayoutInfo.PartitionEntry[0].StartingOffset.QuadPart = offset;   // 1048576i64
    driveLayoutInfo.PartitionEntry[0].PartitionLength.QuadPart = partitionUsableSize;
    driveLayoutInfo.PartitionEntry[0].PartitionNumber = 1;
    driveLayoutInfo.PartitionEntry[0].RewritePartition = TRUE;
    driveLayoutInfo.PartitionEntry[0].Gpt.PartitionType = PARTITION_BASIC_DATA_GUID;
    // driveLayoutInfo.PartitionEntry[0].Gpt.Attributes = GPT_BASIC_DATA_ATTRIBUTE_NO_DRIVE_LETTER;
    wcscpy_s(driveLayoutInfo.PartitionEntry[0].Gpt.Name, L"Microsoft Basic Data");
    THROW_IF_WIN32_ERROR(UuidCreate(&driveLayoutInfo.PartitionEntry[0].Gpt.PartitionId));

    THROW_IF_WIN32_BOOL_FALSE(DeviceIoControl(physicalDrive.get(), IOCTL_DISK_SET_DRIVE_LAYOUT_EX,
                                              (LPVOID)&driveLayoutInfo, sizeof(driveLayoutInfo), NULL, 0,
                                              &bytesReturned, NULL));

    // Call this to tell the system to update its view of this device
    THROW_IF_WIN32_BOOL_FALSE(
        DeviceIoControl(physicalDrive.get(), IOCTL_DISK_UPDATE_PROPERTIES, NULL, 0, NULL, 0, &bytesReturned, NULL));
}

void VirtualDisk::Open()
{
    if (vhd_.is_valid())
    {
        THROW_WIN32(ERROR_INVALID_PARAMETER);
    }

    VIRTUAL_STORAGE_TYPE storageType;
    storageType.DeviceId = VIRTUAL_STORAGE_TYPE_DEVICE_UNKNOWN;
    storageType.VendorId = VIRTUAL_STORAGE_TYPE_VENDOR_UNKNOWN;

    OPEN_VIRTUAL_DISK_PARAMETERS openParameters;
    memset(&openParameters, 0, sizeof(openParameters));
    openParameters.Version = OPEN_VIRTUAL_DISK_VERSION_1;
    openParameters.Version1.RWDepth = 0;
    THROW_IF_WIN32_ERROR(
        OpenVirtualDisk(&storageType, vhdFilename_.c_str(),
                        VIRTUAL_DISK_ACCESS_ATTACH_RO | VIRTUAL_DISK_ACCESS_ATTACH_RW | VIRTUAL_DISK_ACCESS_DETACH,
                        OPEN_VIRTUAL_DISK_FLAG_NONE, &openParameters, vhd_.addressof()));
}

std::wstring VirtualDisk::WaitForVolumeName(long waitTimeMs)
{
    constexpr const int waitInterval = 500;
    long i{ waitTimeMs / waitInterval };
    do
    {
        try
        {
            return GetVolumeName();
        }
        catch (...)
        {
        }

        Sleep(waitInterval);
    } while (--i > 0);

    THROW_WIN32(ERROR_FILE_NOT_FOUND);
}
