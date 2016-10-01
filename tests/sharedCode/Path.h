#pragma once
#include <string>

class Path
{
public:
	static std::wstring Combine(const std::wstring& s1, const std::wstring& s2);
	static std::wstring EnsureQuoted(const std::wstring& s);
	static std::wstring GetModuleFilename();
	static std::wstring GetModulePath();
	static std::wstring GetWorkingDirectory();
};
