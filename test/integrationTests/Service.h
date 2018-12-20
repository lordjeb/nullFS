#pragma once
#include <Windows.h>
#include <string>
#include <win32cpp/handle.h>

#define SERVICE_NOT_FOUND 0

class Service
{
public:
    static std::wstring EnsureQuoted(const std::wstring& s);
    static int GetStatus(const std::wstring& serviceName);
    static void InstallDriver(const std::wstring& infFile);
    static void Start(const std::wstring& serviceName);
    static void Stop(const std::wstring& serviceName);
    static void UninstallDriver(const std::wstring& infFile);

private:
    static void InstallInfSection(const std::wstring& section, const std::wstring& infFile);
    static win32cpp::unique_service_handle Open(const std::wstring& serviceName, DWORD desiredAccess = SERVICE_QUERY_STATUS);
};
