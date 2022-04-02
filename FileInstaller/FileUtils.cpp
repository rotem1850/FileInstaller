#include <windows.h>
#include <iostream>
#include <shlwapi.h>
#include <memory>
#include <filesystem>

#include "FileInstaller.hpp"

namespace fs = std::filesystem;

namespace FileUtils {
	void copy_file(std::wstring const &source_file_path, std::wstring const &dest_file_path) {
		if (!CopyFileW(source_file_path.c_str(), dest_file_path.c_str(), false)) {
			DEBUG_MSG("CopyFileExW failed with error_code=" << GetLastError() << 
				" path=" << dest_file_path.c_str());
			throw FileInstallerException(FileInstallerStatus::FILEUTILS_COPY_FILE_COPY_FAILED);
		}
	}

	void delete_file(std::wstring const &file_path) {
		bool is_deleted = DeleteFileW(file_path.c_str());
		if (!is_deleted) {
			DEBUG_MSG("DeleteFileW failed with error_code=" << GetLastError() << " path = " << file_path.c_str());
			throw FileInstallerException(FileInstallerStatus::FILEUTILS_DELETE_FILE_DELETE_FAILED);
		}
	}

	void create_directory(std::wstring const &directory_path) {
		if (!CreateDirectoryW(directory_path.c_str(), nullptr)) {
			DEBUG_MSG("CreateDirectoryW failed with error_code=" << GetLastError() <<
				" dir_path= " << directory_path.c_str());
			throw FileInstallerException(FileInstallerStatus::FILEUTILS_CREATE_DIRECTORY_CREATE_FAILED);
		}
	}

	void delete_directory(std::wstring const &directory_path) {
		if (!RemoveDirectoryW(directory_path.c_str())) {
			DEBUG_MSG("RemoveDirectoryW failed with error_code=" << GetLastError() <<
				" dir_path= " << directory_path.c_str());
			throw FileInstallerException(FileInstallerStatus::FILEUTILS_DELETE_DIRECTORY_REMOVE_FAILED);
		}
	}

	std::wstring change_file_path_directory(std::wstring const &source_file_path,
											std::wstring const &new_directory) {
		fs::path file_name = fs::path(source_file_path).filename();
		return fs::path(new_directory) / file_name;
	}

	bool is_path_exists(std::wstring const &path) {
		return PathFileExistsW(path.c_str());
	}
}