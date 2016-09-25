#include "Path.h"
#include <Windows.h>
#include <vector>


std::wstring Path::Combine(const std::wstring& s1, const std::wstring& s2)
{
	auto s{ s1 };
	if (s.back() != L'\\') s += L'\\';
	s += s2;
	return s;
}

std::wstring Path::EnsureQuoted(const std::wstring& s)
{
	auto pos = s.find(L' ');
	return std::wstring::npos == pos ? s : L'\"' + s + L'\"';
}

std::wstring Path::GetModuleFilename()
{
	std::vector<wchar_t> moduleFilename(MAX_PATH);
	::GetModuleFileName(nullptr, &moduleFilename[0], moduleFilename.size());
	return std::wstring(&moduleFilename[0]);
}

std::wstring Path::GetModulePath()
{
	auto moduleFilename = GetModuleFilename();
	auto pos = moduleFilename.find_last_of(L'\\');
	return moduleFilename.substr(0, pos);
}
