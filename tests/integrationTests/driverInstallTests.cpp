#include <gtest\gtest.h>
#include <Windows.h>
#include <SetupAPI.h>
#include "Path.h"
#include "Service.h"
#include "Security.h"

TEST(DriverConfiguration, Installs)
{
	ASSERT_TRUE(Security::IsUserAdmin());

	std::wstring infFilename{ Path::Combine(Path::GetModulePath(), L"nullfs.inf") };

	Service::InstallDriver(infFilename);
	ASSERT_EQ(SERVICE_STOPPED, Service::GetServiceStatus(L"nullFS"));

	Service::UninstallDriver(infFilename);
	ASSERT_EQ(SERVICE_NOT_FOUND, Service::GetServiceStatus(L"nullFS"));
}
