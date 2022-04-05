#include <windows.h>
#include <vector>
#include <iostream>

#include "FileInstaller.hpp"

std::vector<std::wstring> g_installation_files = { L"installation_files\\pch.cpp",
												   L"installation_files\\a.txt",
											       L"installation_files\\poster.zip"
												 };

std::wstring g_installation_target = L"target";

std::vector<std::shared_ptr<ResourceInstaller>> create_resource_installers() {
	std::vector<std::shared_ptr<ResourceInstaller>> resource_installers = {};
	auto dir_install = std::make_shared<DirInstaller>(g_installation_target);
	resource_installers.push_back(dir_install);

	for (auto& file_path : g_installation_files) {
		auto file_install = std::make_shared<FileInstaller>(file_path, g_installation_target);
		resource_installers.push_back(file_install);
	}
	return resource_installers;
}

int main()
{
	std::vector<std::shared_ptr<ResourceInstaller>> resource_installers = create_resource_installers();
	auto status = FileInstallerStatus::FILEINSTALLER_SUCCESS;
	
	try {
		Installer installer(resource_installers);
		installer.install();
	}
	catch (FileInstallerException const &e) {
		status = e.get_status();
	}

	return (int)status;
}