#include <windows.h>
#include <iostream>

#include "FileInstaller.hpp"
#include "FileUtils.hpp"

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
	if (!FileUtils::is_path_exists(this->m_directory_path.c_str())) {
		return;
	}

	FileUtils::delete_directory(this->m_directory_path);
	DEBUG_MSG("Directory was deleted. path=" << this->m_directory_path.c_str());
}