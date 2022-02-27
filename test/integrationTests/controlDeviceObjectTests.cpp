#include "pch.h"
#include <nullFS/names.h>
#include "NullFsDriverEnvironment.h"
#include "CreateTestDisk.h"

using ::testing::Eq;

TEST(FileSystemDeviceObjectTests, Can_open)
{
    const wil::unique_hfile hFS{ CreateFile(NF_WIN32_DEVICE_NAME, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                            nullptr, OPEN_EXISTING, 0, nullptr) };

    ASSERT_THAT(hFS.is_valid(), Eq(true)) << L"GetLastError() == " << GetLastError();
}

struct LogicalVolumeTests : ::testing::Test
{
    LogicalVolumeTests()
        : driveLetter_{ L"N:" },
          createTestDisk_{ NullFsDriverEnvironment::GetWorkingDirectory() + L"\\nullfs.vhd", driveLetter_ }
    {
    }

    std::wstring   driveLetter_;
    CreateTestDisk createTestDisk_;

    [[nodiscard]] std::wstring GetTestFilename(const std::wstring& relativeFilename = L"") const
    {
        return driveLetter_ + L'\\' + relativeFilename;
    }
};

struct UnformattedLogicalVolumeTests : LogicalVolumeTests
{
    UnformattedLogicalVolumeTests()
    {
        createTestDisk_.Setup(false);
    }
};

TEST_F(UnformattedLogicalVolumeTests, Unformatted_volume_is_unrecognized)
{
    const wil::unique_hfile hFS{ CreateFile(GetTestFilename().c_str(), GENERIC_ALL,
                                            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, CREATE_NEW,
                                            0, nullptr) };

    ASSERT_THAT(hFS.is_valid(), Eq(false)) << L"GetLastError() == " << GetLastError();
    ASSERT_THAT(GetLastError(), Eq(static_cast<DWORD>(ERROR_UNRECOGNIZED_VOLUME)));
}

struct FormattedLogicalVolumeTests : LogicalVolumeTests
{
    FormattedLogicalVolumeTests()
    {
        createTestDisk_.Setup(true);
    }
};

TEST_F(FormattedLogicalVolumeTests, Can_open_root_directory)
{
    const wil::unique_hfile testFile{ CreateFile(GetTestFilename().c_str(), GENERIC_ALL,
                                                 FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                                                 CREATE_NEW, 0, nullptr) };

    // BUGBUG: Currently fails with ERROR_FILE_NOT_FOUND because we haven't a directory to open yet
    ASSERT_THAT(testFile.is_valid(), Eq(true)) << L"GetLastError() == " << GetLastError();
}
