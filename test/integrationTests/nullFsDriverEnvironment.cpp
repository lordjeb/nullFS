#include "nullFsDriverEnvironment.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <win32cpp/error.h>
#include <win32cpp/handle.h>
#include <win32cpp/string_extensions.h>
#include "TestDriver.h"
#include "../../src/driver/ioctl.h"

using ::testing::Eq;

enum
{
    NULL_FS_DRIVER_ENVIRONMENT_INSTALLED = 0x01,
    NULL_FS_DRIVER_ENVIRONMENT_STARTED = 0x02
};

NullFsDriverEnvironment::NullFsDriverEnvironment()
    : testDriver_{ L"nullFS", win32cpp::appendPath(getWorkingDirectory(), L"nullfs.inf") }
{
}

NullFsDriverEnvironment::~NullFsDriverEnvironment()
{
}

void NullFsDriverEnvironment::SetUp()
{
    ASSERT_TRUE(isUserAdmin());

    testDriver_.install();
    flags |= NULL_FS_DRIVER_ENVIRONMENT_INSTALLED;

    testDriver_.start();
    flags |= NULL_FS_DRIVER_ENVIRONMENT_STARTED;
}

void NullFsDriverEnvironment::TearDown()
{
    if (flags & NULL_FS_DRIVER_ENVIRONMENT_STARTED)
    {
        // Send hack IOCTL to allow our FS driver to unload
        win32cpp::unique_file_handle hFs{ CreateFile(NF_WIN32_DEVICE_NAME, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr) };
        CHECK_BOOL(bool(hFs));
        EXPECT_THAT(DeviceIoControl(hFs.get(), IOCTL_SHUTDOWN, nullptr, 0, nullptr, 0, nullptr, nullptr), Eq(TRUE)) << L"GetLastError() == " << GetLastError();

        testDriver_.stop();
    }

    if (flags & NULL_FS_DRIVER_ENVIRONMENT_INSTALLED)
    {
        testDriver_.uninstall();
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
