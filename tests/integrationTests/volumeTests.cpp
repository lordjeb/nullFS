#include <gtest\gtest.h>
#include <Windows.h>
#include "..\..\driver\names.h"

#define SERVICE_DELAY 1000

TEST(ControlDeviceObjectTests, Can_open)
{
	HANDLE hFs = CreateFile(L"\\\\.\\" NF_NAME, GENERIC_ALL, 0, nullptr, OPEN_EXISTING, 0, nullptr);
	EXPECT_NE(hFs, INVALID_HANDLE_VALUE);
	if (INVALID_HANDLE_VALUE != hFs)
	{
		CloseHandle(hFs);
	}
}
