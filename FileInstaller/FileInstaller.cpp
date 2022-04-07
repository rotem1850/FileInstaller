#include <windows.h>
#include <iostream>

#include "FileInstaller.hpp"
#include "FileUtils.hpp"

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
