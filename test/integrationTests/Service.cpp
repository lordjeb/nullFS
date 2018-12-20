#include "Service.h"
#include <Windows.h>
#include <SetupAPI.h>
#include <win32cpp/error.h>
#include <win32cpp/handle.h>

#define QUOTE L'\"'

std::wstring Service::EnsureQuoted(const std::wstring& s)
{
    auto pos = s.find(L' ');
    return std::wstring::npos == pos ? s : QUOTE + s + QUOTE;
}

int Service::GetStatus(const std::wstring& serviceName)
{
    int result = SERVICE_NOT_FOUND;
    auto service = Open(serviceName.c_str());
    if (service)
    {
        SERVICE_STATUS status = { 0 };
        if (QueryServiceStatus(service.get(), &status))
        {
            result = status.dwCurrentState;
        }
    }

    return result;
}

void Service::InstallDriver(const std::wstring& infFile)
{
    return InstallInfSection(L"DefaultInstall", infFile);
}

void Service::InstallInfSection(const std::wstring& section, const std::wstring& infFile)
{
    auto command = section + L" 128 " + EnsureQuoted(infFile);
    ::InstallHinfSection(nullptr, nullptr, command.c_str(), 0);
}

win32cpp::unique_service_handle Service::Open(const std::wstring& serviceName, DWORD desiredAccess /*= SERVICE_QUERY_STATUS*/)
{
    win32cpp::unique_service_handle scm{ OpenSCManager(nullptr, nullptr, desiredAccess) };
    CHECK_BOOL(bool(scm));
    return win32cpp::unique_service_handle{ OpenService(scm.get(), serviceName.c_str(), desiredAccess) };
}

void Service::Start(const std::wstring& serviceName)
{
    auto service = Open(serviceName, SERVICE_START);
    if (service)
    {
        StartService(service.get(), 0, nullptr);
    }
}

void Service::Stop(const std::wstring& serviceName)
{
    auto service = Open(serviceName, SERVICE_STOP);
    if (service)
    {
        SERVICE_STATUS status = { 0 };
        ControlService(service.get(), SERVICE_CONTROL_STOP, &status);
    }
}

void Service::UninstallDriver(const std::wstring& infFile)
{
    return InstallInfSection(L"DefaultUninstall", infFile);
}
