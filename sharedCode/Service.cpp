#include "Service.h"
#include <Windows.h>
#include <SetupAPI.h>
#include "Path.h"

int Service::GetServiceStatus(const std::wstring& serviceName)
{
	int result = SERVICE_NOT_FOUND;
	auto scm = OpenSCManager(nullptr, nullptr, SERVICE_QUERY_STATUS);
	if (scm)
	{
		auto service = OpenService(scm, serviceName.c_str(), SERVICE_QUERY_STATUS);
		if (service)
		{
			SERVICE_STATUS status = { 0 };
			if (QueryServiceStatus(service, &status))
			{
				result = status.dwCurrentState;
			}
			CloseServiceHandle(service);
		}
		CloseServiceHandle(scm);
	}

	return result;
}

void Service::InstallDriver(const std::wstring& infFile)
{
	return InstallInfSection(L"DefaultInstall", infFile);
}

void Service::InstallInfSection(const std::wstring& section, const std::wstring& infFile)
{
	auto command = section + L" 128 " + Path::EnsureQuoted(infFile);
	::InstallHinfSection(nullptr, nullptr, command.c_str(), 0);
}

void Service::UninstallDriver(const std::wstring& infFile)
{
	return InstallInfSection(L"DefaultUninstall", infFile);
}
