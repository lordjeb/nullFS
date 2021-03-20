#include "pch.h"
#include "NullFsDriverEnvironment.h"

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new NullFsDriverEnvironment);
    return RUN_ALL_TESTS();
}
