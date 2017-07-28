#pragma once

#if defined(DBG)
#define NfDbgPrint(lvl, fmt, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, lvl, fmt, ##__VA_ARGS__)
#else
#define NfDbgPrint(x)
#endif
