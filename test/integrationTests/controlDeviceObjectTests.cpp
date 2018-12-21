#include "pch.h"
#include "../../src/driver/names.h"

using ::testing::Eq;

TEST(FileSystemDeviceObjectTests, Can_open)
{
    win32cpp::unique_file_handle hFs{ CreateFile(NF_WIN32_DEVICE_NAME, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                                 nullptr, OPEN_EXISTING, 0, nullptr) };

    ASSERT_THAT(bool(hFs), Eq(true));
}

TEST(VolumeTests, Can_open_volume)
{
    win32cpp::unique_file_handle hFs{ CreateFile(NF_WIN32_DISK_DEVICE_NAME, GENERIC_ALL,
                                                 FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0,
                                                 nullptr) };

    ASSERT_THAT(bool(hFs), Eq(true));
}
