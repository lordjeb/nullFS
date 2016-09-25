#include <gtest\gtest.h>
#include "Path.h"


TEST(Path, GetModuleFilename_returns_full_path)
{
	auto fn = Path::GetModuleFilename();
	ASSERT_GT(fn.length(), 4U);
	ASSERT_EQ(L':', fn.at(1));
	ASSERT_EQ(0, _wcsicmp(L".exe", fn.substr(fn.length() - 4).c_str()));
}

TEST(Path, GetModulePath_returns_directory)
{
	auto fn = Path::GetModulePath();
	ASSERT_NE(0, _wcsicmp(L".exe", fn.substr(fn.length() - 4).c_str()));
}

TEST(Path, GetModulePath_not_ends_with_slash)
{
	auto fn = Path::GetModulePath();
	ASSERT_NE(L'\\', fn.at(fn.length()-1));
}

TEST(Path, Combine_adds_slash)
{
	auto fn = Path::Combine(L"C:\\directory", L"filename.txt");
	ASSERT_STREQ(L"C:\\directory\\filename.txt", fn.c_str());
}

TEST(Path, Combine_not_adds_slash)
{
	auto fn = Path::Combine(L"C:\\directory\\", L"filename.txt");
	ASSERT_STREQ(L"C:\\directory\\filename.txt", fn.c_str());

	fn = Path::Combine(L"C:\\directory", L"\\filename.txt");
	ASSERT_STREQ(L"C:\\directory\\filename.txt", fn.c_str());

	fn = Path::Combine(L"C:\\directory\\", L"\\filename.txt");
	ASSERT_STREQ(L"C:\\directory\\filename.txt", fn.c_str());
}

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
