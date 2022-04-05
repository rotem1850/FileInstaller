#include <windows.h>
#include <iostream>
#include <vector>

#include "FileInstaller.hpp"
#include "FileUtils.hpp"

Installer::Installer(std::vector<std::shared_ptr<ResourceInstaller>>& resource_installers) 
	:m_resource_installers(resource_installers),
	m_installed_resource_installers({}),
	m_is_installed(false) {}

Installer::~Installer() {
	if (!this->m_is_installed) {
		this->_revert();
	}
}

void Installer::install() {
	for (auto& resource_installer : this->m_resource_installers) {
		resource_installer->install();
		this->m_installed_resource_installers.push_back(resource_installer);
	}

	this->m_is_installed = true;
}

void Installer::_revert() {
	DEBUG_MSG("Reverting installation (best effort)");
	// We revert in reverse order so the files will be deleted first and then the directories.
	for (auto resource_installer = this->m_installed_resource_installers.rbegin(); resource_installer != this->m_installed_resource_installers.rend(); ++resource_installer)
	{
		(*resource_installer)->revert();
	}
}

/*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
FileInstaller::FileInstaller(std::wstring const &file_path, std::wstring const &installation_dir) 
	:m_file_path(file_path),
	m_installation_dir(installation_dir),
	m_is_file_already_exists(true) {}

FileInstaller::~FileInstaller() {}

void FileInstaller::install() {
	this->_copy_file();
}

void FileInstaller::revert() {
	if (!this->m_is_file_already_exists) {
		this->_delete_file();
	}
}

void FileInstaller::_copy_file() {
	std::wstring full_target_path = FileUtils::change_file_path_directory(this->m_file_path, this->m_installation_dir);

	bool is_file_exists = FileUtils::is_path_exists(full_target_path);
	if (!is_file_exists) {
		this->m_is_file_already_exists = false;
		FileUtils::copy_file(this->m_file_path, full_target_path);
		DEBUG_MSG("File was copied. from_path=" << this->m_file_path.c_str() << ", to_path=" << full_target_path.c_str());
	}
	else {
		DEBUG_MSG("File already exists. path=" << full_target_path.c_str());
	}
}

void FileInstaller::_delete_file() {
	std::wstring full_target_path = FileUtils::change_file_path_directory(this->m_file_path, this->m_installation_dir);
	FileUtils::delete_file(full_target_path, true);
	DEBUG_MSG("File was deleted path=" << full_target_path.c_str());
}



DirInstaller::DirInstaller(std::wstring const& directory_path)
	:m_directory_path(directory_path),
	m_is_directory_already_exists(true) {}

DirInstaller::~DirInstaller() {}

void DirInstaller::install() {
	this->_create_directory();
}

void DirInstaller::revert() {
	if (!this->m_is_directory_already_exists) {
		this->_delete_directory();
	}
}

void DirInstaller::_create_directory() {
	if (!FileUtils::is_path_exists(this->m_directory_path.c_str())) {
		this->m_is_directory_already_exists = false;
		FileUtils::create_directory(this->m_directory_path);
		DEBUG_MSG("Directory was created. path=" << this->m_directory_path.c_str());
	}
	else {
		DEBUG_MSG("Installation dir already exists. path=" << this->m_directory_path.c_str());
	}
}

void DirInstaller::_delete_directory() {
	auto status = FileInstallerStatus::FILEINSTALLER_SUCCESS;

	if (!FileUtils::is_path_exists(this->m_directory_path.c_str())) {
		return;
	}

	FileUtils::delete_directory(this->m_directory_path);
	DEBUG_MSG("Directory was deleted. path=" << this->m_directory_path.c_str());
}