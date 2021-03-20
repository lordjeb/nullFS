#include "InstallTestDriver.h"
#include <string>
#include <SetupAPI.h>

#define QUOTE L'\"'

InstallTestDriver::InstallTestDriver(const std::wstring& serviceName, const std::wstring& infFile)
    : serviceName_{ serviceName }, infFile_{ infFile }
{
}

InstallTestDriver::~InstallTestDriver()
{
    if (installed_)
    {
        uninstall();
    }
}

std::wstring InstallTestDriver::ensureQuoted(const std::wstring& s)
{
    auto pos = s.find(L' ');
    return std::wstring::npos == pos ? s : QUOTE + s + QUOTE;
}

void InstallTestDriver::install()
{
    installInfSection(L"DefaultInstall");
    // TODO: Detect error and throw

    installed_ = true;
}

void InstallTestDriver::installInfSection(const std::wstring& section)
{
    auto command = section + L" 128 " + ensureQuoted(infFile_);
    InstallHinfSection(nullptr, nullptr, command.c_str(), 0);
}

void InstallTestDriver::uninstall()
{
    installInfSection(L"DefaultUninstall");
    // TODO: Detect error and throw
}
