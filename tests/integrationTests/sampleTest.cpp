#include <gtest\gtest.h>
#include <Windows.h>
#include <SetupAPI.h>
#include "Path.h"
#include "Service.h"
#include "Security.h"

TEST(DriverConfiguration, Installs)
{
	ASSERT_TRUE(Security::IsUserAdmin());

	std::wstring infFilename{ Path::EnsureQuoted(Path::Combine(Path::GetModulePath(), L"nullfs.inf")) };

	std::wstring command{ L"DefaultInstall 128 " };
	command += infFilename;
	InstallHinfSection(nullptr, nullptr, command.c_str(), 0);

	ASSERT_EQ(SERVICE_STOPPED, Service::GetServiceStatus(L"nullFS"));

	command = L"DefaultUninstall 128 ";
	command += infFilename;
	InstallHinfSection(nullptr, nullptr, command.c_str(), 0);

	ASSERT_EQ(SERVICE_NOT_FOUND, Service::GetServiceStatus(L"nullFS"));
}
