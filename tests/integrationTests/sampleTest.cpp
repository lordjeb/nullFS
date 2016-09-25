#include <gtest\gtest.h>
#include <Windows.h>
#include <SetupAPI.h>
#include "Path.h"


#define SERVICE_NOT_FOUND 0

int GetServiceStatus(const std::wstring& serviceName)
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

TEST(DriverConfiguration, Installs)
{
	// TODO: Assert that we have admin access!
	std::wstring infFilename{ Path::EnsureQuoted(Path::Combine(Path::GetModulePath(), L"nullfs.inf")) };

	std::wstring command{ L"DefaultInstall 128 " };
	command += infFilename;
	InstallHinfSection(nullptr, nullptr, command.c_str(), 0);

	ASSERT_EQ(SERVICE_STOPPED, GetServiceStatus(L"nullFS"));

	command = L"DefaultUninstall 128 ";
	command += infFilename;
	InstallHinfSection(nullptr, nullptr, command.c_str(), 0);

	ASSERT_EQ(SERVICE_NOT_FOUND, GetServiceStatus(L"nullFS"));
}
