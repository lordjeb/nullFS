#include <gtest\gtest.h>
#include "Path.h"


TEST(Path, EnsureQuoted_when_space)
{
	auto fn = Path::EnsureQuoted(L"C:\\test filename.txt");
	ASSERT_STREQ(L"\"C:\\test filename.txt\"", fn.c_str());
}

TEST(Path, EnsureQuoted_not_when_no_space)
{
	auto fn = Path::EnsureQuoted(L"C:\\test.txt");
	ASSERT_STREQ(L"C:\\test.txt", fn.c_str());
}
