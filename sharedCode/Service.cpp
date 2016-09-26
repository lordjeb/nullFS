#include "Service.h"
#include <Windows.h>

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
