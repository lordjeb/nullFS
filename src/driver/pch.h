#pragma once

#include <ntifs.h>
#include <ntdddisk.h>
#include <TraceLoggingProvider.h>
#include <winmeta.h>

#include "support.h"
#include "flowControl.h"
#include "globalData.h"
#include "traceLogging.h"
#include "pooltags.h"

#define WIL_KERNEL_MODE
#include <wil/resource.h>
