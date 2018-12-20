#include "Service.h"
#include <Windows.h>
#include <SetupAPI.h>

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
        if (QueryServiceStatus(service, &status))
        {
            result = status.dwCurrentState;
        }
        CloseServiceHandle(service);
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

SC_HANDLE Service::Open(const std::wstring& serviceName, DWORD desiredAccess /*= SERVICE_QUERY_STATUS*/)
{
    SC_HANDLE result = nullptr;
    auto scm = OpenSCManager(nullptr, nullptr, desiredAccess);
    if (scm)
    {
        result = OpenService(scm, serviceName.c_str(), desiredAccess);
        CloseServiceHandle(scm);
    }

    return result;
}

void Service::Start(const std::wstring& serviceName)
{
    auto service = Open(serviceName, SERVICE_START);
    if (service)
    {
        StartService(service, 0, nullptr);
        CloseServiceHandle(service);
    }
}

void Service::Stop(const std::wstring& serviceName)
{
    auto service = Open(serviceName, SERVICE_STOP);
    if (service)
    {
        SERVICE_STATUS status = { 0 };
        ControlService(service, SERVICE_CONTROL_STOP, &status);
        CloseServiceHandle(service);
    }
}

void Service::UninstallDriver(const std::wstring& infFile)
{
    return InstallInfSection(L"DefaultUninstall", infFile);
}
