#pragma once
#include <gtest/gtest.h>

class NullFsDriverEnvironment : public ::testing::Environment
{
public:
    NullFsDriverEnvironment() : flags(0) {}
    virtual ~NullFsDriverEnvironment() {}
    virtual void SetUp();
    virtual void TearDown();

private:
    unsigned long flags;
};
