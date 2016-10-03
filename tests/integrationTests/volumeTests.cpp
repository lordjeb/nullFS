#include <gtest/gtest.h>
#include <Windows.h>
#include "../../driver/names.h"
#include "../../driver/ioctl.h"

//TEST(ControlDeviceObjectTests, Can_open)
//{
//    auto hFs = CreateFile(NF_WIN32_DEVICE_NAME, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
//    EXPECT_NE(hFs, INVALID_HANDLE_VALUE);
//    if (INVALID_HANDLE_VALUE != hFs)
//    {
//        CloseHandle(hFs);
//    }
//}

TEST(ControlDeviceObjectTests, Can_call_unregister)
{
    auto hFs = CreateFile(NF_WIN32_DEVICE_NAME, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
    EXPECT_NE(hFs, INVALID_HANDLE_VALUE);
    if (INVALID_HANDLE_VALUE != hFs)
    {
        EXPECT_TRUE(DeviceIoControl(hFs, IOCTL_SHUTDOWN, nullptr, 0, nullptr, 0, nullptr, nullptr)) << L"GetLastError() == " << GetLastError();
        CloseHandle(hFs);
    }
}
