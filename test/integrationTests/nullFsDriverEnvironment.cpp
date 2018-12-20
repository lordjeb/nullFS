#include "nullFsDriverEnvironment.h"
#include <gtest/gtest.h>
#include <win32cpp/string_extensions.h>
#include "Service.h"
#include "../../src/driver/ioctl.h"

#define NULL_FS_DRIVER_ENVIRONMENT_INSTALLED 0x01
#define NULL_FS_DRIVER_ENVIRONMENT_STARTED 0x02

void NullFsDriverEnvironment::SetUp()
{
    ASSERT_TRUE(isUserAdmin());

    auto infFilename{ win32cpp::appendPath(getWorkingDirectory(), L"nullfs.inf") };

    Service::InstallDriver(infFilename);
    ASSERT_EQ(SERVICE_STOPPED, Service::GetStatus(L"nullFS"));
    flags |= NULL_FS_DRIVER_ENVIRONMENT_INSTALLED;

    Service::Start(L"nullFS");
    ASSERT_EQ(SERVICE_RUNNING, Service::GetStatus(L"nullFS"));
    flags |= NULL_FS_DRIVER_ENVIRONMENT_STARTED;
}

void NullFsDriverEnvironment::TearDown()
{
    if (flags & NULL_FS_DRIVER_ENVIRONMENT_STARTED)
    {
        auto hFs = CreateFile(NF_WIN32_DEVICE_NAME, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
        EXPECT_NE(hFs, INVALID_HANDLE_VALUE);
        if (INVALID_HANDLE_VALUE != hFs)
        {
            EXPECT_TRUE(DeviceIoControl(hFs, IOCTL_SHUTDOWN, nullptr, 0, nullptr, 0, nullptr, nullptr)) << L"GetLastError() == " << GetLastError();
            CloseHandle(hFs);
        }

        Service::Stop(L"nullFS");
        EXPECT_EQ(SERVICE_STOPPED, Service::GetStatus(L"nullFS"));
    }

    if (flags & NULL_FS_DRIVER_ENVIRONMENT_INSTALLED)
    {
        auto infFilename{ win32cpp::appendPath(getWorkingDirectory(), L"nullfs.inf") };

        Service::UninstallDriver(infFilename);
        EXPECT_EQ(SERVICE_NOT_FOUND, Service::GetStatus(L"nullFS"));
    }
}

std::wstring NullFsDriverEnvironment::getWorkingDirectory()
{
    std::vector<wchar_t> workingDirectory(MAX_PATH);
    ::GetCurrentDirectory(static_cast<DWORD>(workingDirectory.size()), &workingDirectory[0]);
    return std::wstring{ &workingDirectory[0] };
}

bool NullFsDriverEnvironment::isUserAdmin()
{
    BOOL b;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    PSID AdministratorsGroup;
    b = AllocateAndInitializeSid(
        &NtAuthority,
        2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &AdministratorsGroup);
    if (b)
    {
        if (!CheckTokenMembership(nullptr, AdministratorsGroup, &b))
        {
            b = FALSE;
        }
        FreeSid(AdministratorsGroup);
    }

    return FALSE != b;
}
