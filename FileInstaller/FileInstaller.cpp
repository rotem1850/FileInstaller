#include <windows.h>
#include <iostream>
#include <pathcch.h>
#include <shlwapi.h>
#include <vector>
#include <cstdint>

#include "FileInstaller.hpp"

FileInstaller::FileInstaller(std::vector<LPCWSTR> &file_paths, LPCWSTR installation_dir)
	: file_paths(file_paths),
	installation_dir(installation_dir),
	is_dir_already_exists(false),
	file_paths_to_clean({}) {

}

FileInstaller::~FileInstaller() {

}

void FileInstaller::copy_file(LPCWSTR file_path) {
	wchar_t full_target_path[MAX_PATH] = { 0 };
	LPCWSTR filename = PathFindFileNameW(file_path);
	HRESULT result = PathCchCombine(full_target_path, MAX_PATH, this->installation_dir, filename);
	if (S_OK != result) {
		DEBUG_MSG("PathCchCombine failed with error_code=" << result << " path=" << file_path);
		throw FileInstallerException(FileInstallerStatus::FILEINSTALLER_COPY_FILE_JOIN_PATH_FAILED);
	}

	if (!CopyFileExW(file_path, full_target_path, nullptr, nullptr, nullptr, COPY_FILE_FAIL_IF_EXISTS)) {
		if (GetLastError() != ERROR_FILE_EXISTS) {
			DEBUG_MSG("CopyFileExW failed with error_code=" << GetLastError() << " path=" << full_target_path);
			throw FileInstallerException(FileInstallerStatus::FILEINSTALLER_COPY_FILE_COPY_FAILED);
		}

		/* File already exists, so we have nothing to do. */
		DEBUG_MSG("Ignoring already exising file. path=" << full_target_path);
	}
	else {
		/* The file doesn't exists in the target dir. We save it so we could delete it later. */
		this->file_paths_to_clean.push_back(file_path);
		DEBUG_MSG("File was copied. from_path=" << file_path << ", to_path=" << full_target_path);
	}

}

void FileInstaller::copy_files() {
	for (auto file_path = std::begin(this->file_paths); file_path != std::end(this->file_paths); ++file_path) {
		this->copy_file(*file_path);
	}
}

void FileInstaller::delete_file(LPCWSTR file_path) {
	wchar_t full_target_path[MAX_PATH] = { 0 };
	LPCWSTR filename = PathFindFileNameW(file_path);
	HRESULT result = PathCchCombine(full_target_path, MAX_PATH, this->installation_dir, filename);
	if (S_OK != result) {
		DEBUG_MSG("PathCchCombine failed with error_code=" << result);
		throw FileInstallerException(FileInstallerStatus::FILEINSTALLER_DELETE_FILE_JOIN_PATH_FAILED);
	}
	
	bool is_deleted = DeleteFileW(full_target_path);
	if (!is_deleted) {
		DEBUG_MSG("DeleteFileW failed with error_code=" << GetLastError() << " path = " << full_target_path);
		throw FileInstallerException(FileInstallerStatus::FILEINSTALLER_DELETE_FILE_DELETE_FAILED);
	}
	DEBUG_MSG("File was deleted path=" << full_target_path);
}

void FileInstaller::delete_installed_files() {
	FileInstallerStatus status = FileInstallerStatus::FILEINSTALLER_SUCCESS;
	for (auto file_path = std::begin(this->file_paths_to_clean); file_path != std::end(this->file_paths_to_clean); ++file_path) {
		try {
			this->delete_file(*file_path);
		}
		catch (FileInstallerException &e) {
			/* Critical error. Nothing to do */
			status = e.get_status();
		}
	}

	if (FileInstallerStatus::FILEINSTALLER_SUCCESS != status) {
		throw FileInstallerException(status);
	}
}

void FileInstaller::create_installation_dir() {
	if (!CreateDirectoryW(this->installation_dir, NULL)) {
		if (GetLastError() == ERROR_ALREADY_EXISTS) {
			this->is_dir_already_exists = true;
			DEBUG_MSG("Installation dir already exists. path=" << this->installation_dir);
		}
		else {
			DEBUG_MSG("CreateDirectoryW failed with error_code=" << GetLastError() << " dir_path= " << this->installation_dir);
			throw FileInstallerException(FileInstallerStatus::FILEINSTALLER_CREATE_INSTALLATION_DIR_CREATE_FAILED);
		}
	}
}

void FileInstaller::delete_installation_dir() {
	if (this->is_dir_already_exists) {
		return;
	}

	if (!RemoveDirectoryW(this->installation_dir)) {
		DEBUG_MSG("RemoveDirectoryW failed with error_code=" << GetLastError() << " dir_path= " << this->installation_dir);
		throw FileInstallerException(FileInstallerStatus::FILEINSTALLER_DELETE_INSTALLATION_DIR_REMOVE_FAILED);
	}

	DEBUG_MSG("Directory was deleted. path=" << this->installation_dir);
}

void FileInstaller::install() {
	try {
		this->create_installation_dir();
		this->copy_files();
	}
	catch (FileInstallerException &e) {
		this->revert_installation();
		/* Installing Exception will be ignored in case an exception will be raised during revertion. */
		throw e;
	}
}

void FileInstaller::revert_installation() {
	DEBUG_MSG("Reverting installation");
	this->delete_installed_files();
	this->delete_installation_dir();
}