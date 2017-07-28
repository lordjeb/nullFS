#pragma once

#define DPFLTR_DEVICE_CONTROL	(DPFLTR_MASK | 0x01)
#define DPFLTR_CREATE			(DPFLTR_MASK | 0x02)
#define DPFLTR_CLEANUP			(DPFLTR_MASK | 0x04)
#define DPFLTR_CLOSE			(DPFLTR_MASK | 0x08)
#define DPFLTR_CLOSE			(DPFLTR_MASK | 0x08)
#define DPFLTR_FS_CONTROL		(DPFLTR_MASK | 0x10)

#if defined(DBG)
#define NfDbgPrint(lvl, fmt, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, lvl, fmt, ##__VA_ARGS__)
#else
#define NfDbgPrint(x)
#endif
