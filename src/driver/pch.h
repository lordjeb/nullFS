#pragma once

#include <ntifs.h>
#include <ntdddisk.h>

#include "debug.h"
#include "support.h"
#include "flowControl.h"
#include "globalData.h"
#include "pooltags.h"

#define WIL_KERNEL_MODE
#include <wil/resource.h>
