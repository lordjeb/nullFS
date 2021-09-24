#include "pch.h"
#include <nullFS/names.h>
#include "NullFsDriverEnvironment.h"
#include "CreateTestDisk.h"

using ::testing::Eq;

TEST(FileSystemDeviceObjectTests, Can_open)
{
    wil::unique_hfile hFS{ CreateFile(NF_WIN32_DEVICE_NAME, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                                      OPEN_EXISTING, 0, nullptr) };

    ASSERT_THAT(hFS.is_valid(), Eq(true)) << L"GetLastError() == " << GetLastError();
}

struct LogicalVolumeTests : public ::testing::Test
{
    LogicalVolumeTests()
        : driveLetter_{ L"N:" },
          createTestDisk_{ NullFsDriverEnvironment::getWorkingDirectory() + L"\\nullfs.vhd", driveLetter_ }
    {
    }

    virtual ~LogicalVolumeTests() = default;

    std::wstring driveLetter_;
    CreateTestDisk createTestDisk_;

    std::wstring getTestFilename(const std::wstring& relativeFilename = L"")
    {
        return driveLetter_ + L'\\' + relativeFilename;
    }
};

struct UnformattedLogicalVolumeTests : public LogicalVolumeTests
{
    UnformattedLogicalVolumeTests()
    {
        createTestDisk_.setup(false);
    }

    virtual ~UnformattedLogicalVolumeTests() = default;
};

TEST_F(UnformattedLogicalVolumeTests, Unformatted_volume_is_unrecognized)
{
    wil::unique_hfile hFS{ CreateFile(getTestFilename().c_str(), GENERIC_ALL,
                                      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, CREATE_NEW, 0,
                                      nullptr) };

    ASSERT_THAT(hFS.is_valid(), Eq(false)) << L"GetLastError() == " << GetLastError();
    ASSERT_THAT(GetLastError(), Eq((DWORD)ERROR_UNRECOGNIZED_VOLUME));
}

struct FormattedLogicalVolumeTests : public LogicalVolumeTests
{
    FormattedLogicalVolumeTests()
    {
        createTestDisk_.setup(true);
    }

    virtual ~FormattedLogicalVolumeTests() = default;
};

TEST_F(FormattedLogicalVolumeTests, Can_open_root_directory)
{
    wil::unique_hfile testFile{ CreateFile(getTestFilename().c_str(), GENERIC_ALL,
                                           FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, CREATE_NEW,
                                           0, nullptr) };

    // BUGBUG: Currently fails with ERROR_FILE_NOT_FOUND because we haven't a directory to open yet
    ASSERT_THAT(testFile.is_valid(), Eq(true)) << L"GetLastError() == " << GetLastError();
}
