#pragma once
#include "names.h"

#define NF_IOCTL(index) CTL_CODE(NF_DEVICE_TYPE, (index), METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SHUTDOWN NF_IOCTL(0x01)
