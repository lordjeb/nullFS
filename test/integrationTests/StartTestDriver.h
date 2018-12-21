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

    win32cpp::unique_service_handle open(DWORD desiredAccess = SERVICE_QUERY_STATUS);
    void stop();
};
