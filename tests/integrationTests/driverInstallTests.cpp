#include <gtest\gtest.h>
#include <Windows.h>
#include "Path.h"
#include "Service.h"
#include "Security.h"

TEST(DriverConfiguration, Installs_and_uninstalls)
{
	ASSERT_TRUE(Security::IsUserAdmin());

	auto infFilename{ Path::Combine(Path::GetModulePath(), L"nullfs.inf") };

	Service::InstallDriver(infFilename);
	ASSERT_EQ(SERVICE_STOPPED, Service::GetStatus(L"nullFS"));

	Service::UninstallDriver(infFilename);
	ASSERT_EQ(SERVICE_NOT_FOUND, Service::GetStatus(L"nullFS"));
}

TEST(DriverConfiguration, Starts_and_stops)
{
	ASSERT_TRUE(Security::IsUserAdmin());

	auto infFilename{ Path::Combine(Path::GetModulePath(), L"nullfs.inf") };

	Service::InstallDriver(infFilename);
	ASSERT_EQ(SERVICE_STOPPED, Service::GetStatus(L"nullFS"));

	Service::Start(L"nullFS");
	ASSERT_EQ(SERVICE_RUNNING, Service::GetStatus(L"nullFS"));

	Service::Stop(L"nullFS");
	ASSERT_EQ(SERVICE_STOPPED, Service::GetStatus(L"nullFS"));

	Service::UninstallDriver(infFilename);
	ASSERT_EQ(SERVICE_NOT_FOUND, Service::GetStatus(L"nullFS"));
}
