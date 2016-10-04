#include <gtest/gtest.h>
#include <Windows.h>
#include "Security.h"
#include "Path.h"
#include "Service.h"
#include "../../driver/names.h"
#include "../../driver/ioctl.h"

class ControlDeviceObjectTests : public ::testing::Test
{
protected:

    static void SetUpTestCase()
    {
        ASSERT_TRUE(Security::IsUserAdmin());
        
        auto infFilename{ Path::Combine(Path::GetWorkingDirectory(), L"nullfs.inf") };
        
        Service::InstallDriver(infFilename);
        ASSERT_EQ(SERVICE_STOPPED, Service::GetStatus(L"nullFS"));
        
        Service::Start(L"nullFS");
        ASSERT_EQ(SERVICE_RUNNING, Service::GetStatus(L"nullFS"));
    }

    static void TearDownTestCase()
    {
        auto hFs = CreateFile(NF_WIN32_DEVICE_NAME, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
        EXPECT_NE(hFs, INVALID_HANDLE_VALUE);
        if (INVALID_HANDLE_VALUE != hFs)
        {
            EXPECT_TRUE(DeviceIoControl(hFs, IOCTL_SHUTDOWN, nullptr, 0, nullptr, 0, nullptr, nullptr)) << L"GetLastError() == " << GetLastError();
            CloseHandle(hFs);
        }

        Service::Stop(L"nullFS");
        ASSERT_EQ(SERVICE_STOPPED, Service::GetStatus(L"nullFS"));

        auto infFilename{ Path::Combine(Path::GetWorkingDirectory(), L"nullfs.inf") };

        Service::UninstallDriver(infFilename);
        ASSERT_EQ(SERVICE_NOT_FOUND, Service::GetStatus(L"nullFS"));
    }
};

TEST_F(ControlDeviceObjectTests, Can_open)
{
    auto hFs = CreateFile(NF_WIN32_DEVICE_NAME, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
    EXPECT_NE(hFs, INVALID_HANDLE_VALUE);
    if (INVALID_HANDLE_VALUE != hFs)
    {
        CloseHandle(hFs);
    }
}
