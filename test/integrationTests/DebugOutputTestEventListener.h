#pragma once

class DebugOutputTestEventListener : public ::testing::EmptyTestEventListener
{
public:
    void OnTestStart(const ::testing::TestInfo& test_info) override;
    void OnTestEnd(const ::testing::TestInfo& test_info) override;
    void OnTestSuiteStart(const ::testing::TestSuite& test_suite) override;
    void OnTestSuiteEnd(const ::testing::TestSuite& test_suite) override;
};
