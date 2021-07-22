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

void CreateTestDisk::setup(bool formatDisk /*= true*/)
{
    virtualDisk_.Delete();
    virtualDisk_.Create(32 * 1024 * 1024);
    created_ = true;

    virtualDisk_.AssignDriveLetter(driveLetter_);

    if (formatDisk)
    {
        auto volumeName = LR"(\\.\)" + driveLetter_;

        wil::unique_hfile volume{ CreateFile(volumeName.c_str(), GENERIC_ALL,
                                             FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                                             OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, nullptr) };
        THROW_LAST_ERROR_IF(!volume.is_valid());

        DWORD bytesWritten;
        struct
        {
            unsigned long volumeSignature = 0xbeeff11e;   // BEEF FILE? Clever.
            BYTE padding[508] = { 0 };
        } firstSector;

        THROW_IF_WIN32_BOOL_FALSE(WriteFile(volume.get(), &firstSector, sizeof(firstSector), &bytesWritten, nullptr));
    }
}

void CreateTestDisk::teardown()
{
    // Sleep for a short bit to give mount operations a chance to finish? Probably not necessary, but it seems to avoid
    // a STATUS_NO_SUCH_DEVICE error that happens because we are detaching while it is still trying to work?
    //
    // I don't know if this is really necessary
    Sleep(500);

    virtualDisk_.Detach();
    virtualDisk_.Delete();
}
