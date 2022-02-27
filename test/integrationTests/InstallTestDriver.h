#pragma once

class InstallTestDriver
{
public:
    InstallTestDriver(const std::wstring& serviceName, const std::wstring& infFile);
    ~InstallTestDriver();

    void install();

private:
    bool         installed_{ false };
    std::wstring serviceName_;
    std::wstring infFile_;

    static std::wstring EnsureQuoted(const std::wstring& s);
    void                InstallInfSection(const std::wstring& section);
    void                Uninstall();
};
