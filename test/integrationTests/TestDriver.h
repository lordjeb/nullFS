#pragma once
#include <Windows.h>
#include <string>
#include <win32cpp/handle.h>

#define SERVICE_NOT_FOUND 0

class TestDriver
{
public:
    TestDriver(const std::wstring& serviceName, const std::wstring& infFile);

    void install();
    void start();
    void stop();
    void uninstall();

    template<typename Fn>
    static DWORD waitForServiceStatus(const win32cpp::unique_service_handle& service, Fn fn)
    {
        SERVICE_STATUS status = { 0 };
        do
        {
            QueryServiceStatus(service.get(), &status);

        } while (!fn(status.dwCurrentState));
        return status.dwCurrentState;
    }

private:
    std::wstring serviceName_;
    std::wstring infFile_;

    std::wstring ensureQuoted(const std::wstring& s);
    void installInfSection(const std::wstring& section);
    win32cpp::unique_service_handle open(DWORD desiredAccess = SERVICE_QUERY_STATUS);
};
