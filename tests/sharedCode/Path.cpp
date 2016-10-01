#include "Path.h"
#include <Windows.h>
#include <vector>

#define PATH_SEP L'\\'
#define QUOTE L'\"'

std::wstring Path::Combine(const std::wstring& s1, const std::wstring& s2)
{
	auto pos1 = s1.find_last_not_of(PATH_SEP);
	auto pos2 = s2.find_first_not_of(PATH_SEP);
	return s1.substr(0, pos1 + 1) + PATH_SEP + s2.substr(pos2);
}

std::wstring Path::EnsureQuoted(const std::wstring& s)
{
	auto pos = s.find(L' ');
	return std::wstring::npos == pos ? s : QUOTE + s + QUOTE;
}

std::wstring Path::GetModuleFilename()
{
	std::vector<wchar_t> moduleFilename(MAX_PATH);
	::GetModuleFileName(nullptr, &moduleFilename[0], static_cast<DWORD>(moduleFilename.size()));
	return std::wstring(&moduleFilename[0]);
}

std::wstring Path::GetModulePath()
{
	auto moduleFilename = GetModuleFilename();
	auto pos = moduleFilename.find_last_of(PATH_SEP);
	return moduleFilename.substr(0, pos);
}

std::wstring Path::GetWorkingDirectory()
{
	std::vector<wchar_t> workingDirectory(MAX_PATH);
	::GetCurrentDirectory(static_cast<DWORD>(workingDirectory.size()), &workingDirectory[0]);
	return std::wstring(&workingDirectory[0]);
}
