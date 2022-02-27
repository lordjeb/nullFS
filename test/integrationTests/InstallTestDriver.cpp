#include "pch.h"
#include "InstallTestDriver.h"
#include <string>
#include <SetupAPI.h>

InstallTestDriver::InstallTestDriver(const std::wstring& serviceName, const std::wstring& infFile)
    : serviceName_{ serviceName }, infFile_{ infFile }
{
}

InstallTestDriver::~InstallTestDriver()
{
    if (installed_)
    {
        Uninstall();
    }
}

std::wstring InstallTestDriver::EnsureQuoted(const std::wstring& s)
{
    constexpr wchar_t QUOTE = L'\"';
    const auto        pos = s.find(L' ');
    return std::wstring::npos == pos ? s : QUOTE + s + QUOTE;
}

void InstallTestDriver::install()
{
    InstallInfSection(L"DefaultInstall");
    // TODO: Detect error and throw

    installed_ = true;
}

void InstallTestDriver::InstallInfSection(const std::wstring& section)
{
    const auto command = section + L" 128 " + EnsureQuoted(infFile_);
#pragma warning(suppress : 6387)   // This usage conforms to the examples for the function
    InstallHinfSection(nullptr, nullptr, command.c_str(), 0);
}

void InstallTestDriver::Uninstall()
{
    InstallInfSection(L"DefaultUninstall");
    // TODO: Detect error and throw
}
