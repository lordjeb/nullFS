#pragma once
#include <ntifs.h>

typedef USHORT NodeTypeCode;

constexpr NodeTypeCode NfNodeTypeCodeVolumeControlBlock = 0x0F01;

inline NodeTypeCode NodeType(void* ptr)
{
    return *static_cast<NodeTypeCode*>(ptr);
}