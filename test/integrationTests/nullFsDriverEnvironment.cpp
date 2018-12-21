#include "pch.h"
#include "nullFsDriverEnvironment.h"
#include "../../src/driver/ioctl.h"

using ::testing::Eq;

constexpr const wchar_t driverServiceName[] = NF_NAME;

NullFsDriverEnvironment::NullFsDriverEnvironment()
    : installTestDriver_{ driverServiceName, win32cpp::appendPath(getWorkingDirectory(), L"nullfs.inf") },
      startTestDriver_{ driverServiceName }
{
}

void NullFsDriverEnvironment::SetUp()
{
    ASSERT_TRUE(isUserAdmin());
    installTestDriver_.install();
    startTestDriver_.start();
}

void NullFsDriverEnvironment::TearDown()
{
    // Send hack IOCTL to allow our FS driver to unload
    win32cpp::unique_file_handle hFs{ CreateFile(NF_WIN32_DEVICE_NAME, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                                 nullptr, OPEN_EXISTING, 0, nullptr) };
    if (hFs)
    {
        EXPECT_THAT(DeviceIoControl(hFs.get(), IOCTL_SHUTDOWN, nullptr, 0, nullptr, 0, nullptr, nullptr), Eq(TRUE))
            << L"GetLastError() == " << GetLastError();
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
    b = AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0,
                                 0, &AdministratorsGroup);
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
