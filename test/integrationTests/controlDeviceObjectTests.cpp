#include <wil/resource.h>
#include "../../src/driver/names.h"

using ::testing::Eq;

TEST(FileSystemDeviceObjectTests, Can_open)
{
    wil::unique_hfile hFS{ CreateFile(NF_WIN32_DEVICE_NAME, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                                      OPEN_EXISTING, 0, nullptr) };

    ASSERT_THAT(hFS.is_valid(), Eq(true));
}

TEST(VolumeTests, Can_open_volume)
{
    wil::unique_hfile hFS{ CreateFile(NF_WIN32_DISK_DEVICE_NAME, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                      nullptr, OPEN_EXISTING, 0, nullptr) };

    ASSERT_THAT(hFS.is_valid(), Eq(true));
}
