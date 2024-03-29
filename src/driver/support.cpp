#include "pch.h"
#include "support.h"
#include "GlobalData.h"

NTSTATUS NfCompleteRequest(_In_ PIRP irp, const NTSTATUS status, const ULONG_PTR information)
{
    irp->IoStatus.Status = status;
    irp->IoStatus.Information = information;
    IoCompleteRequest(irp, IO_DISK_INCREMENT);
    return status;
}

BOOLEAN NfDeviceIsFileSystemDeviceObject(_In_ PDEVICE_OBJECT deviceObject)
{
    return deviceObject == GlobalData.fileSystemDeviceObject;
}
