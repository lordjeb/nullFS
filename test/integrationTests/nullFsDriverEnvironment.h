#pragma once
#include "CreateTestDisk.h"
#include "InstallTestDriver.h"
#include "StartTestDriver.h"

class NullFsDriverEnvironment : public ::testing::Environment
{
public:
    NullFsDriverEnvironment();

    virtual void SetUp();
    virtual void TearDown();
    std::wstring getWorkingDirectory();
    bool isUserAdmin();

private:
    CreateTestDisk createTestDisk_;
    InstallTestDriver installTestDriver_;
    StartTestDriver startTestDriver_;
};
