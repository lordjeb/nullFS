#include "TestDriver.h"
#include <Windows.h>
#include <SetupAPI.h>
#include <win32cpp/error.h>
#include <win32cpp/handle.h>

#define QUOTE L'\"'

TestDriver::TestDriver(const std::wstring& serviceName, const std::wstring& infFile)
    : serviceName_{ serviceName }, infFile_{ infFile }
{
}

std::wstring TestDriver::ensureQuoted(const std::wstring& s)
{
    auto pos = s.find(L' ');
    return std::wstring::npos == pos ? s : QUOTE + s + QUOTE;
}

void TestDriver::install()
{
    installInfSection(L"DefaultInstall");
    // TODO: Detect error and throw
}

void TestDriver::start()
{
    auto service = open(SERVICE_START);
    CHECK_BOOL(StartService(service.get(), 0, nullptr));
    auto status = waitForServiceStatus(service, [](DWORD status) { return status != SERVICE_START_PENDING; });
    CHECK_EQ(status, DWORD{ SERVICE_RUNNING });
}

void TestDriver::stop()
{
    auto service = open(SERVICE_STOP);
    SERVICE_STATUS ss = { 0 };
    CHECK_BOOL(ControlService(service.get(), SERVICE_CONTROL_STOP, &ss));
    waitForServiceStatus(service, [](DWORD status) { return status == SERVICE_STOPPED; });
}

void TestDriver::installInfSection(const std::wstring& section)
{
    auto command = section + L" 128 " + ensureQuoted(infFile_);
    ::InstallHinfSection(nullptr, nullptr, command.c_str(), 0);
}

void TestDriver::uninstall()
{
    installInfSection(L"DefaultUninstall");
    // TODO: Detect error and throw
}

win32cpp::unique_service_handle TestDriver::open(DWORD desiredAccess /*= SERVICE_QUERY_STATUS*/)
{
    win32cpp::unique_service_handle scm{ OpenSCManager(nullptr, nullptr, desiredAccess) };
    CHECK_BOOL(bool(scm));
    win32cpp::unique_service_handle service{ OpenService(scm.get(), serviceName_.c_str(), desiredAccess) };
    CHECK_BOOL(bool(service));
    return service;
}
