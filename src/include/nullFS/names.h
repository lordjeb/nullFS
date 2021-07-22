#pragma once

#define NF_NAME L"nullFS"
#define NF_DRIVER_DEVICE_NAME L"\\" NF_NAME
#define NF_DRIVER_SYMBOLIC_NAME L"\\DosDevices\\" NF_NAME
#define NF_WIN32_DEVICE_NAME L"\\\\.\\" NF_NAME

#define NF_DEVICE_TYPE FILE_DEVICE_DISK_FILE_SYSTEM

#define NF_DISK_NAME L"N:"
#define NF_DRIVER_DISK_DEVICE_NAME L"\\VolumeN"
#define NF_DRIVER_DISK_SYMBOLIC_NAME L"\\DosDevices\\" NF_DISK_NAME
#define NF_WIN32_DISK_DEVICE_NAME L"\\\\.\\" NF_DISK_NAME

struct NfFirstSector
{
    unsigned long volumeSignature;
    unsigned char padding[508];
};
