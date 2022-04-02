#include <windows.h>
#include <iostream>
#include <pathcch.h>
#include <shlwapi.h>
#include <vector>
#include <cstdint>

#include "FileInstaller.hpp"

namespace FileUtils {
	void copy_file(std::wstring const &source_file_path, std::wstring const &dest_file_path) {
		if (!CopyFileW(source_file_path.c_str(), dest_file_path.c_str(), false)) {
			DEBUG_MSG("CopyFileExW failed with error_code=" << GetLastError() << " path=" << dest_file_path.c_str());
			throw FileInstallerException(FileInstallerStatus::FILEUTILS_COPY_FILE_COPY_FAILED);
		}
	}

	void delete_file(std::wstring const &file_path) {
		bool is_deleted = DeleteFileW(file_path.c_str());
		if (!is_deleted) {
			DEBUG_MSG("DeleteFileW failed with error_code=" << GetLastError() << " path = " << file_path.c_str());
			throw FileInstallerException(FileInstallerStatus::FILEUTILS_DELETE_FILE_DELETE_FAILED);
		}

		DEBUG_MSG("File was deleted path=" << file_path.c_str());
	}

	void create_directory(std::wstring const &directory_path) {
		if (!CreateDirectoryW(directory_path.c_str(), nullptr)) {
			DEBUG_MSG("CreateDirectoryW failed with error_code=" << GetLastError() <<
					  " dir_path= " << directory_path.c_str());
			throw FileInstallerException(FileInstallerStatus::FILEUTILS_CREATE_DIRECTORY_CREATE_FAILED);
		}

		DEBUG_MSG("Directory was created. path=" << directory_path.c_str());
	}

	void delete_directory(std::wstring const &directory_path) {
		if (!RemoveDirectoryW(directory_path.c_str())) {
			DEBUG_MSG("RemoveDirectoryW failed with error_code=" << GetLastError() <<
					  " dir_path= " << directory_path.c_str());
			throw FileInstallerException(FileInstallerStatus::FILEUTILS_DELETE_DIRECTORY_REMOVE_FAILED);
		}

		DEBUG_MSG("Directory was deleted. path=" << directory_path.c_str());
	}

	void change_file_path_directory(std::wstring const &source_file_path,
									 std::wstring const &new_directory, 
									 wchar_t *new_file_path) {
		wchar_t full_target_path[MAX_PATH] = { 0 };
		wchar_t *filename = PathFindFileNameW(source_file_path.c_str());

		if (nullptr == new_file_path) {
			throw FileInstallerException(FileInstallerStatus::FILEUTILS_CHANGE_FILE_PATH_DIRECTORY_INVALID_ARGUMENT);
		}

		HRESULT result = PathCchCombine(new_file_path, ARRAYSIZE(full_target_path), new_directory.c_str(), filename);
		DEBUG_MSG("PathCchCombine " << full_target_path << " path=" << new_directory.c_str() << new_file_path);
		if (FAILED(result)) {
			DEBUG_MSG("PathCchCombine failed with error_code=" << result << " path=" << source_file_path.c_str());
			throw FileInstallerException(FileInstallerStatus::FILEUTILS_CHANGE_FILE_PATH_DIRECTORY_JOIN_PATH_FAILED);
		}
	}

}

FileInstaller::FileInstaller(std::vector<std::wstring> &file_paths, std::wstring installation_dir)
	: m_file_paths(file_paths),
	m_installation_dir(installation_dir),
	m_is_dir_already_exists(false),
	m_file_paths_to_clean({}) {}

FileInstaller::~FileInstaller() {
	
}

void FileInstaller::_copy_file(std::wstring const &file_path) {
	wchar_t full_target_path[MAX_PATH] = { 0 };
	FileUtils::change_file_path_directory(file_path, this->m_installation_dir, full_target_path);

	bool is_file_exists = PathFileExistsW(full_target_path);
	if (!is_file_exists) {
		this->m_file_paths_to_clean.push_back(file_path.c_str());
		FileUtils::copy_file(file_path, full_target_path);
		DEBUG_MSG("File was copied. from_path=" << file_path.c_str() << ", to_path=" << full_target_path);
	}
	else {
		DEBUG_MSG("File already exists. path=" << full_target_path);
	}
}

void FileInstaller::_copy_files() {
	for (auto &file_path : this->m_file_paths) {
		this->_copy_file(file_path);
	}
}

void FileInstaller::_delete_file(std::wstring const &file_path) {
	wchar_t full_target_path[MAX_PATH] = { 0 };

	FileUtils::change_file_path_directory(file_path, this->m_installation_dir, full_target_path);
	FileUtils::delete_file(full_target_path);
}

FileInstallerStatus FileInstaller::_delete_installed_files() {
	auto status = FileInstallerStatus::FILEINSTALLER_SUCCESS;
	for (auto &file_path : this->m_file_paths_to_clean) {
		try {
			this->_delete_file(file_path);
		}
		catch (FileInstallerException &e) {
			// We're deleting files by "best effort".
			status = e.get_status();
		}
	}

	return status;
}

void FileInstaller::_create_installation_dir() {
	if (!PathFileExists(this->m_installation_dir.c_str())) {
		FileUtils::create_directory(this->m_installation_dir);
	}
	else {
		DEBUG_MSG("Installation dir already exists. path=" << this->m_installation_dir.c_str());
	}

}

FileInstallerStatus FileInstaller::_delete_installation_dir() {
	auto status = FileInstallerStatus::FILEINSTALLER_SUCCESS;

	if (this->m_is_dir_already_exists) {
		return status;
	}

	if (!PathFileExists(this->m_installation_dir.c_str())) {
		return status;
	}

	try {
		FileUtils::delete_directory(this->m_installation_dir);
	}
	catch (FileInstallerException &e) {
		status = e.get_status();
	}

	return status;
}

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
	(void)this->_delete_installation_dir();
}