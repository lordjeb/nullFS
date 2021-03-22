#include "pch.h"
#include <wil/resource.h>
#include <nullFS/names.h>
#include <conio.h>

using ::testing::Eq;

TEST(FileSystemDeviceObjectTests, Can_open)
{
    wil::unique_hfile hFS{ CreateFile(NF_WIN32_DEVICE_NAME, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                                      OPEN_EXISTING, 0, nullptr) };

    ASSERT_THAT(hFS.is_valid(), Eq(true)) << L"GetLastError() == " << GetLastError();
}

TEST(LogicalVolumeTests, Can_open_root)
{
    // TODO: Enable priv
    wil::unique_hfile hFS{ CreateFile(L"N:\\test.txt", GENERIC_ALL,
                                      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, CREATE_NEW,
                                      0, nullptr) };

    ASSERT_THAT(hFS.is_valid(), Eq(true)) << L"GetLastError() == " << GetLastError();
}
