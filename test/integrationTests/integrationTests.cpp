#include "pch.h"
#include <format>
#include "NullFsDriverEnvironment.h"

class DebugOutputTestEventListener : public ::testing::EmptyTestEventListener
{
    void OnTestStart(const ::testing::TestInfo& test_info) override
    {
        auto out = std::format("++++ {}\n", test_info.name());
        OutputDebugStringA(out.c_str());
    }

    void OnTestEnd(const ::testing::TestInfo& test_info) override
    {
        auto out = std::format("---- {}\n", test_info.name());
        OutputDebugStringA(out.c_str());
    }

    void OnTestSuiteStart(const ::testing::TestSuite& test_suite) override
    {
        auto out = std::format("-- {}\n", test_suite.name());
        OutputDebugStringA(out.c_str());
    }

    void OnTestSuiteEnd(const ::testing::TestSuite& test_suite) override
    {
        auto out = std::format("-- {}\n", test_suite.name());
        OutputDebugStringA(out.c_str());
    }
};

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    ::testing::AddGlobalTestEnvironment(new NullFsDriverEnvironment);

    ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();

    // Adds listener to the end. googletest takes the ownership of the ptr.
    listeners.Append(new DebugOutputTestEventListener);

    return RUN_ALL_TESTS();
}
