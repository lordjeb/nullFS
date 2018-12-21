#include "pch.h"
#include "support.h"

NTSTATUS NfCompleteRequest(_In_ PIRP irp, NTSTATUS status, ULONG_PTR information)
{
    irp->IoStatus.Status = status;
    irp->IoStatus.Information = information;
    IoCompleteRequest(irp, IO_NO_INCREMENT);
    return status;
}

BOOLEAN NfDeviceIsFileSystemDeviceObject(_In_ PDEVICE_OBJECT deviceObject)
{
    return deviceObject == globalData.fileSystemDeviceObject;
}

BOOLEAN NfDeviceIsDiskDeviceObject(_In_ PDEVICE_OBJECT deviceObject)
{
    return deviceObject == globalData.diskDeviceObject;
}
