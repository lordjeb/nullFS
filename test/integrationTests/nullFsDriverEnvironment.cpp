#include "pch.h"
#include "nullFsDriverEnvironment.h"
#include <nullFS/ioctl.h>

using ::testing::Eq;

constexpr const wchar_t DriverServiceName[] = NF_NAME;

NullFsDriverEnvironment::NullFsDriverEnvironment()
    : installTestDriver_{ DriverServiceName, GetWorkingDirectory() + L"\\nullfs.inf" },
      startTestDriver_{ DriverServiceName }
{
}

void NullFsDriverEnvironment::SetUp()
{
    ASSERT_TRUE(IsUserAdmin());
    installTestDriver_.install();
    startTestDriver_.Start();
}

void NullFsDriverEnvironment::TearDown()
{
    // Send hack IOCTL to allow our FS driver to unload
    const wil::unique_handle hFs{ CreateFile(NF_WIN32_DEVICE_NAME, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                             nullptr, OPEN_EXISTING, 0, nullptr) };
    if (hFs.is_valid())
    {
        EXPECT_THAT(DeviceIoControl(hFs.get(), IOCTL_NULLFS_SHUTDOWN, nullptr, 0, nullptr, 0, nullptr, nullptr),
                    Eq(TRUE))
            << L"GetLastError() == " << GetLastError();
    }
}

std::wstring NullFsDriverEnvironment::GetWorkingDirectory()
{
    std::vector<wchar_t> workingDirectory(MAX_PATH);
    ::GetCurrentDirectory(static_cast<DWORD>(workingDirectory.size()), &workingDirectory[0]);
    return std::wstring{ &workingDirectory[0] };
}

bool NullFsDriverEnvironment::IsUserAdmin()
{
    BOOL                     b;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    PSID                     administratorsGroup;

    b = AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0,
                                 0, &administratorsGroup);
    if (b)
    {
        if (!CheckTokenMembership(nullptr, administratorsGroup, &b))
        {
            b = FALSE;
        }
        FreeSid(administratorsGroup);
    }

    return FALSE != b;
}
