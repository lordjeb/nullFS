#include "Path.h"
#include <Windows.h>
#include <vector>

#define PATH_SEP L'\\'
#define QUOTE L'\"'

std::wstring Path::EnsureQuoted(const std::wstring& s)
{
	auto pos = s.find(L' ');
	return std::wstring::npos == pos ? s : QUOTE + s + QUOTE;
}

std::wstring Path::GetWorkingDirectory()
{
	std::vector<wchar_t> workingDirectory(MAX_PATH);
	::GetCurrentDirectory(static_cast<DWORD>(workingDirectory.size()), &workingDirectory[0]);
	return std::wstring(&workingDirectory[0]);
}
