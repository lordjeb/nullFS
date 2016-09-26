#pragma once
#include <string>

#define SERVICE_NOT_FOUND 0

class Service
{
public:
	static int GetServiceStatus(const std::wstring& serviceName);
};
