#pragma once
#include <ntifs.h>


#define NF_GLOBAL_DATA_FLAGS_RESOURCE_INITIALIZED       0x01
#define NF_GLOBAL_DATA_FLAGS_DRIVER_DEVICE_CREATED      0x02
#define NF_GLOBAL_DATA_FLAGS_SYMBOLIC_LINK_CREATED      0x04
#define NF_GLOBAL_DATA_FLAGS_FILE_SYSTEM_REGISTERED     0x08
#define NF_GLOBAL_DATA_FLAGS_DISK_DRIVER_DEVICE_CREATED 0x10
#define NF_GLOBAL_DATA_FLAGS_DISK_SYMBOLIC_LINK_CREATED 0x20

// Holds all global data for the driver in a single structure
typedef struct _NfGlobalData
{
	// NF_GLOBAL_DATA_FLAGS_*
	ULONG flags;

	ERESOURCE lock;

	PDRIVER_OBJECT driverObject;

	PDEVICE_OBJECT fileSystemDeviceObject;

	PDEVICE_OBJECT diskDeviceObject;

} NfGlobalData;

extern NfGlobalData globalData;


typedef struct _NfVolumeControlBlock
{
	ULONG n;
} NfVolumeControlBlock;


// The Volume Device Object is an I/O system device object with a workqueue and an VCB record
// appended to the end. There are multiple of these records, one for every mounted volume, and
// are created during a volume mount operation. The work queue is for handling an overload of
// work requests to the volume.
typedef struct _NfVolumeDeviceObject
{
	DEVICE_OBJECT DeviceObject;

	// The following field tells how many requests for this volume have either been enqueued to
	// ExWorker threads or are currently being serviced by ExWorker threads. If the number goes
	// above a certain threshold, put the request on the overflow queue to be executed later.
	ULONG PostedRequestCount;

	// The following field indicates the number of IRP's waiting to be serviced in the overflow queue.
	ULONG OverflowQueueCount;

	// The following field contains the queue header of the overflow queue. The Overflow queue is a
	// list of IRP's linked via the IRP's ListEntry field.
	LIST_ENTRY OverflowQueue;

	// The following spinlock protects access to all the above fields.
	KSPIN_LOCK OverflowQueueSpinLock;

	// This is a common head for the FAT volume file
	FSRTL_COMMON_FCB_HEADER VolumeFileHeader;

	// This is the file system specific volume control block.
	NfVolumeControlBlock Vcb;

} NfVolumeDeviceObject;
