#pragma once

class InstallTestDriver
{
public:
    InstallTestDriver(const std::wstring& serviceName, const std::wstring& infFile);
    ~InstallTestDriver();

    void install();

private:
    bool installed_{ false };
    std::wstring serviceName_;
    std::wstring infFile_;

    std::wstring ensureQuoted(const std::wstring& s);
    void installInfSection(const std::wstring& section);
    void uninstall();
};
