#include "pch.h"
#include "cacheManager.h"

BOOLEAN NfCmAcquireNoOp(PVOID context, BOOLEAN wait)
{
    UNREFERENCED_PARAMETER(context);
    UNREFERENCED_PARAMETER(wait);
    return TRUE;
}

void NfCmReleaseNoOp(PVOID context)
{
    UNREFERENCED_PARAMETER(context);
    return;
}
