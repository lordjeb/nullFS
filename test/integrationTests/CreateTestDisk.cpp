#include "pch.h"
#include "CreateTestDisk.h"
#include <initguid.h>
#include <virtdisk.h>

using ::testing::Eq;

CreateTestDisk::CreateTestDisk(const std::wstring& vhdFilename, const std::wstring& driveLetter)
    : virtualDisk_{ vhdFilename }, driveLetter_{ driveLetter }
{
}

CreateTestDisk::~CreateTestDisk()
{
    if (created_)
    {
        teardown();
    }
}

void CreateTestDisk::setup()
{
    virtualDisk_.Delete();
    virtualDisk_.Create(32 * 1024 * 1024);

    // TODO: Format the disk so it is something we can recognize in our mount

    virtualDisk_.AssignDriveLetter(driveLetter_);

    created_ = true;
}

void CreateTestDisk::teardown()
{
    virtualDisk_.Detach();
    virtualDisk_.Delete();
}
