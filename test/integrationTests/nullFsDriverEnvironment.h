#pragma once
#include <gtest/gtest.h>

class NullFsDriverEnvironment : public ::testing::Environment
{
public:
    NullFsDriverEnvironment() {}
    virtual ~NullFsDriverEnvironment() {}
    virtual void SetUp();
    virtual void TearDown();
    bool IsUserAdmin();
    std::wstring GetWorkingDirectory();
private:
    unsigned long flags{ 0 };
};
