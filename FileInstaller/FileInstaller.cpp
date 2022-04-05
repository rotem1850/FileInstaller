#include <windows.h>
#include <iostream>
#include <vector>

#include "FileInstaller.hpp"
#include "FileUtils.hpp"

FileInstaller::FileInstaller(std::vector<std::wstring> &file_paths, std::wstring const &installation_dir)
	: m_file_paths(file_paths),
	m_installation_dir(installation_dir),
	m_is_dir_already_exists(false),
	m_file_paths_to_clean({}) {}

FileInstaller::~FileInstaller() {}

void FileInstaller::install() {
	try {
		this->_create_installation_dir();
		this->_copy_files();
	}
	catch (FileInstallerException &e) {
		this->_revert_installation();
		// Installation exception will be ignored in case an exception will be thrown during revertion.
		throw e;
	}
}

void FileInstaller::_revert_installation() {
	DEBUG_MSG("Reverting installation (best effort)");
	// Because we're doing best effort we're not return any return values about the status of the revert operation.
	(void)this->_delete_installed_files();

	if (!this->m_is_dir_already_exists) {
		(void)this->_delete_installation_dir();
	}
}

void FileInstaller::_copy_file(std::wstring const &file_path) {
	std::wstring full_target_path = FileUtils::change_file_path_directory(file_path, this->m_installation_dir);

	bool is_file_exists = FileUtils::is_path_exists(full_target_path);
	if (!is_file_exists) {
		this->m_file_paths_to_clean.push_back(file_path.c_str());
		FileUtils::copy_file(file_path, full_target_path);
		DEBUG_MSG("File was copied. from_path=" << file_path.c_str() << ", to_path=" << full_target_path.c_str());
	}
	else {
		DEBUG_MSG("File already exists. path=" << full_target_path.c_str());
	}
}

void FileInstaller::_copy_files() {
	for (auto &file_path : this->m_file_paths) {
		this->_copy_file(file_path);
	}
}

void FileInstaller::_delete_file(std::wstring const &file_path) {
	std::wstring full_target_path = FileUtils::change_file_path_directory(file_path, this->m_installation_dir);
	FileUtils::delete_file(full_target_path, true);
	DEBUG_MSG("File was deleted path=" << full_target_path.c_str());
}

FileInstallerStatus FileInstaller::_delete_installed_files() {
	auto status = FileInstallerStatus::FILEINSTALLER_SUCCESS;
	for (auto &file_path : this->m_file_paths_to_clean) {
		try {
			this->_delete_file(file_path);
		}
		catch (FileInstallerException const &e) {
			// Best effort
			status = e.get_status();
		}
	}

	return status;
}

void FileInstaller::_create_installation_dir() {
	if (!FileUtils::is_path_exists(this->m_installation_dir.c_str())) {
		FileUtils::create_directory(this->m_installation_dir);
		DEBUG_MSG("Directory was created. path=" << this->m_installation_dir.c_str());
	}
	else {
		this->m_is_dir_already_exists = true;
		DEBUG_MSG("Installation dir already exists. path=" << this->m_installation_dir.c_str());
	}
}

FileInstallerStatus FileInstaller::_delete_installation_dir() {
	auto status = FileInstallerStatus::FILEINSTALLER_SUCCESS;

	if (!FileUtils::is_path_exists(this->m_installation_dir.c_str())) {
		return status;
	}

	try {
		FileUtils::delete_directory(this->m_installation_dir);
		DEBUG_MSG("Directory was deleted. path=" << this->m_installation_dir.c_str());
	}
	catch (FileInstallerException const &e) {
		status = e.get_status();
	}

	return status;
}
