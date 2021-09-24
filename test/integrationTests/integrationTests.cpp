#include "pch.h"
#include "NullFsDriverEnvironment.h"
#include "DebugOutputTestEventListener.h"

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    ::testing::AddGlobalTestEnvironment(new NullFsDriverEnvironment);

    ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();

    // Adds listener to the end. googletest takes the ownership of the ptr.
    listeners.Append(new DebugOutputTestEventListener);

    return RUN_ALL_TESTS();
}
