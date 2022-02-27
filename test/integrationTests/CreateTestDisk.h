#pragma once
#include "VirtualDisk.h"

class CreateTestDisk
{
public:
    CreateTestDisk(const std::wstring& vhdFilename, const std::wstring& driveLetter);
    virtual ~CreateTestDisk();

    void Setup(bool formatDisk = true);

private:
    bool               created_{ false };
    VirtualDisk        virtualDisk_;
    const std::wstring driveLetter_;
    void               Teardown();
};