#include "pch.h"
#include "StartTestDriver.h"
#include <string>
#include <Windows.h>

template <typename Fn>
DWORD WaitForServiceStatus(const SC_HANDLE& hService, Fn fn)
{
    SERVICE_STATUS status = { 0 };
    do
    {
        THROW_IF_WIN32_BOOL_FALSE(QueryServiceStatus(hService, &status));

    } while (!fn(status.dwCurrentState));
    return status.dwCurrentState;
}

StartTestDriver::StartTestDriver(const std::wstring& serviceName) : serviceName_{ serviceName }
{
}

StartTestDriver::~StartTestDriver()
{
    if (started_)
    {
        Stop();
    }
}

wil::unique_schandle StartTestDriver::Open(DWORD desiredAccess /*= SERVICE_QUERY_STATUS*/) const
{
    const wil::unique_schandle scm{ OpenSCManager(nullptr, nullptr, desiredAccess) };
    THROW_LAST_ERROR_IF(!scm.is_valid());
    wil::unique_schandle service{ OpenService(scm.get(), serviceName_.c_str(), desiredAccess) };
    THROW_LAST_ERROR_IF(!service.is_valid());
    return service;
}

void StartTestDriver::Start()
{
    const auto service = Open(SERVICE_START | SERVICE_QUERY_STATUS);
    if (StartService(service.get(), 0, nullptr))
    {
        const auto status =
            WaitForServiceStatus(service.get(), [](const DWORD status) { return status != SERVICE_START_PENDING; });
        if (SERVICE_RUNNING != status)
        {
            THROW_WIN32(ERROR_SERVICE_NEVER_STARTED);
        }
    }
    else
    {
        const auto dw = GetLastError();
        if (dw != ERROR_SERVICE_ALREADY_RUNNING)
        {
            THROW_WIN32(dw);
        }
    }

    started_ = true;
}

void StartTestDriver::Stop()
{
    const auto     service = Open(SERVICE_STOP | SERVICE_QUERY_STATUS);
    SERVICE_STATUS ss = { 0 };
    THROW_IF_WIN32_BOOL_FALSE(ControlService(service.get(), SERVICE_CONTROL_STOP, &ss));
    WaitForServiceStatus(service.get(), [](const DWORD status) { return status == SERVICE_STOPPED; });
}
