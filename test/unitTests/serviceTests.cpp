#include <gtest\gtest.h>
#include "Service.h"
#include <Windows.h>


TEST(Service, GetServiceStatus)
{
	ASSERT_EQ(SERVICE_NOT_FOUND, Service::GetStatus(L"48DAAA81_5705_4D65_9BF5_81CDAFA6EDFB"));
	ASSERT_EQ(SERVICE_RUNNING, Service::GetStatus(L"LanmanServer"));
}
