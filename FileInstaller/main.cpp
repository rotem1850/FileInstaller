#include <windows.h>
#include <vector>

#include "FileInstaller.hpp"

std::vector<std::wstring> g_installation_files = { L"installation_files\\pch.cpp",
												   L"installation_files\\a.txt",
											       L"installation_files\\poster.zip" };
std::wstring g_installation_target = L"target";

int main()
{
	auto status = FileInstallerStatus::FILEINSTALLER_SUCCESS;
	try {
		FileInstaller installer = FileInstaller(g_installation_files, g_installation_target);
		installer.install();
	}
	catch(FileInstallerException &e) {
		status = e.get_status();
	}

	return status;
}