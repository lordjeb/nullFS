#include "pch.h"
#include "DebugOutputTestEventListener.h"
#include <format>

void DebugOutputTestEventListener::OnTestStart(const ::testing::TestInfo& test_info)
{
    auto out = std::format("[TEST: +{}]\n", test_info.name());
    OutputDebugStringA(out.c_str());
}

void DebugOutputTestEventListener::OnTestEnd(const ::testing::TestInfo& test_info)
{
    auto out = std::format("[TEST: -{}]\n", test_info.name());
    OutputDebugStringA(out.c_str());
}

void DebugOutputTestEventListener::OnTestSuiteStart(const ::testing::TestSuite& test_suite)
{
    auto out = std::format("[TESTSUITE: +{}]\n", test_suite.name());
    OutputDebugStringA(out.c_str());
}

void DebugOutputTestEventListener::OnTestSuiteEnd(const ::testing::TestSuite& test_suite)
{
    auto out = std::format("[TESTSUITE: -{}]\n", test_suite.name());
    OutputDebugStringA(out.c_str());
}
