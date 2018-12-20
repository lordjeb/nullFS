#include "nullFsDriverEnvironment.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <win32cpp/error.h>
#include <win32cpp/handle.h>
#include <win32cpp/string_extensions.h>
#include "Service.h"
#include "../../src/driver/ioctl.h"

using ::testing::Eq;

enum
{
    NULL_FS_DRIVER_ENVIRONMENT_INSTALLED = 0x01,
    NULL_FS_DRIVER_ENVIRONMENT_STARTED = 0x02
};

void NullFsDriverEnvironment::SetUp()
{
    ASSERT_TRUE(isUserAdmin());

    auto infFilename{ win32cpp::appendPath(getWorkingDirectory(), L"nullfs.inf") };

    Service::InstallDriver(infFilename);
    ASSERT_THAT(Service::GetStatus(L"nullFS"), Eq(SERVICE_STOPPED));
    flags |= NULL_FS_DRIVER_ENVIRONMENT_INSTALLED;

    Service::Start(L"nullFS");
    ASSERT_THAT(Service::GetStatus(L"nullFS"), Eq(SERVICE_RUNNING));
    flags |= NULL_FS_DRIVER_ENVIRONMENT_STARTED;
}

void NullFsDriverEnvironment::TearDown()
{
    if (flags & NULL_FS_DRIVER_ENVIRONMENT_STARTED)
    {
        win32cpp::unique_file_handle hFs{ CreateFile(NF_WIN32_DEVICE_NAME, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr) };
        CHECK_BOOL(bool(hFs));
        EXPECT_THAT(DeviceIoControl(hFs.get(), IOCTL_SHUTDOWN, nullptr, 0, nullptr, 0, nullptr, nullptr), Eq(TRUE)) << L"GetLastError() == " << GetLastError();

        Service::Stop(L"nullFS");
        EXPECT_THAT(Service::GetStatus(L"nullFS"), Eq(SERVICE_STOPPED));
    }

    if (flags & NULL_FS_DRIVER_ENVIRONMENT_INSTALLED)
    {
        auto infFilename{ win32cpp::appendPath(getWorkingDirectory(), L"nullfs.inf") };

        Service::UninstallDriver(infFilename);
        EXPECT_THAT(Service::GetStatus(L"nullFS"), Eq(SERVICE_NOT_FOUND));
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
