#pragma once
#include <gtest/gtest.h>

class NullFsDriverEnvironment : public ::testing::Environment
{
public:
    NullFsDriverEnvironment() {}
    virtual ~NullFsDriverEnvironment() {}
    virtual void SetUp();
    virtual void TearDown();
    bool isUserAdmin();
    std::wstring getWorkingDirectory();
private:
    unsigned long flags{ 0 };
};
