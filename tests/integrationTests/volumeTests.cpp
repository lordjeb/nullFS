#include <gtest\gtest.h>
#include <Windows.h>
#include <Service.h>
#include <Path.h>

class VolumeTests : public ::testing::Test
{
public:
	static void SetUpTestCase()
	{
		auto infFilename{ Path::Combine(Path::GetWorkingDirectory(), L"nullfs.inf") };

		Service::InstallDriver(infFilename);
		ASSERT_EQ(SERVICE_STOPPED, Service::GetStatus(L"nullFS"));

		Service::Start(L"nullFS");
		ASSERT_EQ(SERVICE_RUNNING, Service::GetStatus(L"nullFS"));
	}

	static void TearDownTestCase()
	{
		auto infFilename{ Path::Combine(Path::GetWorkingDirectory(), L"nullfs.inf") };

		Service::Stop(L"nullFS");
		Service::UninstallDriver(infFilename);
	}
};

TEST_F(VolumeTests, Can_open_control_device_object)
{
	HANDLE hFs = CreateFile(L"\\\\.\\nullFS", GENERIC_ALL, 0, nullptr, OPEN_EXISTING, 0, nullptr);
	EXPECT_NE(INVALID_HANDLE_VALUE, hFs);
	if (INVALID_HANDLE_VALUE != hFs)
	{
		CloseHandle(hFs);
	}
}
