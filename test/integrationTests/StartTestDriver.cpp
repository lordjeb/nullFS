#include "pch.h"
#include "StartTestDriver.h"

template <typename Fn>
DWORD waitForServiceStatus(const win32cpp::unique_service_handle& service, Fn fn)
{
    SERVICE_STATUS status = { 0 };
    do
    {
        QueryServiceStatus(service.get(), &status);

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
        stop();
    }
}

win32cpp::unique_service_handle StartTestDriver::open(DWORD desiredAccess /*= SERVICE_QUERY_STATUS*/)
{
    win32cpp::unique_service_handle scm{ OpenSCManager(nullptr, nullptr, desiredAccess) };
    CHECK_BOOL(bool(scm));
    win32cpp::unique_service_handle service{ OpenService(scm.get(), serviceName_.c_str(), desiredAccess) };
    CHECK_BOOL(bool(service));
    return service;
}

void StartTestDriver::start()
{
    auto service = open(SERVICE_START);
    if (StartService(service.get(), 0, nullptr))
    {
        auto status = waitForServiceStatus(service, [](DWORD status) { return status != SERVICE_START_PENDING; });
        CHECK_EQ(status, DWORD{ SERVICE_RUNNING });
    }
    else
    {
        auto dw = GetLastError();
        if (dw != ERROR_SERVICE_ALREADY_RUNNING)
        {
            CHECK_WIN32(dw);
        }
    }

    started_ = true;
}

void StartTestDriver::stop()
{
    auto service = open(SERVICE_STOP);
    SERVICE_STATUS ss = { 0 };
    CHECK_BOOL(ControlService(service.get(), SERVICE_CONTROL_STOP, &ss));
    waitForServiceStatus(service, [](DWORD status) { return status == SERVICE_STOPPED; });
}
