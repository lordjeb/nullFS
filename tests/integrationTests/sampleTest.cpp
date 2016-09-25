#include <gtest\gtest.h>
#include <Windows.h>
#include <SetupAPI.h>
#include "Path.h"


TEST(DriverConfiguration, Installs)
{
	std::wstring infFilename{ Path::EnsureQuoted(Path::Combine(Path::GetModulePath(), L"nullfs.inf")) };

	// TODO: Install the test certificate

	std::wstring command{ L"DefaultInstall 128 " };
	command += infFilename;
	InstallHinfSection(nullptr, nullptr, command.c_str(), 0);

	// TODO: How to determine if it installed and loaded

	command = L"DefaultUninstall 128 ";
	command += infFilename;
	InstallHinfSection(nullptr, nullptr, command.c_str(), 0);

	// TODO: How to determine if it unloaded and uninstalled
}
