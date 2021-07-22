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
    static std::wstring getWorkingDirectory();
    static bool isUserAdmin();

private:
    InstallTestDriver installTestDriver_;
    StartTestDriver startTestDriver_;
};
