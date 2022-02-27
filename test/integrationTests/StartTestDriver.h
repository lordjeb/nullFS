#pragma once

class StartTestDriver
{
public:
    StartTestDriver(const std::wstring& serviceName);
    ~StartTestDriver();

    void Start();

private:
    bool         started_{ false };
    std::wstring serviceName_;

    wil::unique_schandle Open(DWORD desiredAccess = SERVICE_QUERY_STATUS) const;
    void                 Stop();
};
