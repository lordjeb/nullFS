#pragma once
#include "pch.h"

NTSTATUS NfCompleteRequest(_In_ PIRP irp, NTSTATUS status, ULONG_PTR information);
BOOLEAN NfDeviceIsFileSystemDeviceObject(_In_ PDEVICE_OBJECT deviceObject);
BOOLEAN NfDeviceIsDiskDeviceObject(_In_ PDEVICE_OBJECT deviceObject);
