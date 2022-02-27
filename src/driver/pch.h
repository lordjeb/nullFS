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
#pragma warning(push)
#include <CodeAnalysis\Warnings.h>
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
// As of 1.0.220201.1 wil has annotation problems with IRQLs
#pragma warning(disable : 28157 28158 28167)
#include <wil/common.h>
#include <wil/resource.h>
#pragma warning(pop)
