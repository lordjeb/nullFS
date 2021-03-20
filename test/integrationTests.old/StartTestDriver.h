#pragma once

class StartTestDriver
{
public:
    StartTestDriver(const std::wstring& serviceName);
    ~StartTestDriver();

    void start();

private:
    bool started_{ false };
    std::wstring serviceName_;

    wil::unique_schandle open(DWORD desiredAccess = SERVICE_QUERY_STATUS);
    void stop();
};
