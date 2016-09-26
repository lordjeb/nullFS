#pragma once
#include <string>

#define SERVICE_NOT_FOUND 0

class Service
{
public:
	static int GetServiceStatus(const std::wstring& serviceName);
	static void InstallDriver(const std::wstring& infFile);
	static void UninstallDriver(const std::wstring& infFile);

private:
	static void InstallInfSection(const std::wstring& section, const std::wstring& infFile);
};
