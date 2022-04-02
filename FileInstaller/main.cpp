#include <windows.h>
#include <vector>

#include "FileInstaller.hpp"

std::vector<LPCWSTR> installation_files = { L"installation_files\\pch.cpp", 
											L"installation_files\\a.txt",
											L"installation_files\\poster.zip" };
LPCWSTR installation_target = L"target";

int main()
{
	FileInstallerStatus status = FileInstallerStatus::FILEINSTALLER_SUCCESS;
	FileInstaller f = FileInstaller(installation_files, installation_target);
	try {
		f.install();
	}
	catch(FileInstallerException &e) {
		status = e.get_status();
	}

	return status;
}