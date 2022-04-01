// FileInstaller.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include <iostream>
#include <windows.h>
#include <winbase.h>
#include <pathcch.h>
#include <array>
#include <vector>
#include <cstdint>
#include <functional>

LPCWSTR installation_files2[] = {L"pch.cpp", L"a.txt", L"rotem.txt"};
std::vector<LPCWSTR> installation_files = { L"pch.cpp", L"a.txt", L"rotem.txt" };

LPCWSTR installation_dir = L"target";

class FileInstaller {
	public:
		std::vector<LPCWSTR> &file_paths;
		LPCWSTR installation_dir;

		FileInstaller(std::vector<LPCWSTR> &file_paths, LPCWSTR installation_dir);
		~FileInstaller();
		void install();
		
	private:
		bool is_dir_already_exists;
		std::vector<LPCWSTR> file_paths_to_clean;
		void copy_file(LPCWSTR file_path);
		void delete_file(LPCWSTR file_path);
		void copy_files();
		void delete_installed_files();
		void create_installation_dir();
		void delete_installation_dir();

};

void FileInstaller::copy_file(LPCWSTR file_path) {
	wchar_t full_target_path[MAX_PATH] = { 0 };
	HRESULT result = PathCchCombine(full_target_path, MAX_PATH, this->installation_dir, file_path);
	if (S_OK != result) {
		//TODO: raise an exception.
	}

	std::wcout << full_target_path << L"\n";
	if (!CopyFileExW(file_path, full_target_path, nullptr, nullptr, nullptr, COPY_FILE_FAIL_IF_EXISTS)) {
		if (GetLastError() != ERROR_FILE_EXISTS) {
			//TODO: raise an exception
			return;
		}
		std::wcout << L"File exists " << full_target_path << L"\n";
	}
	else {
		/* Save non-existing files in the target dir so we could delete it later. */
		this->file_paths_to_clean.push_back(file_path);
		std::wcout << L"File not exists " << full_target_path << L"\n";
	}

}

void FileInstaller::copy_files() {
	// std::for_each(std::begin(this->file_paths), std::end(this->file_paths), this->copy_file);
	for (auto file_path = std::begin(this->file_paths); file_path != std::end(this->file_paths); ++file_path) {
		this->copy_file(*file_path);
	}
}

void FileInstaller::delete_file(LPCWSTR file_path) {
	wchar_t full_target_path[MAX_PATH] = { 0 };
	HRESULT result = PathCchCombine(full_target_path, MAX_PATH, this->installation_dir, file_path);
	if (S_OK != result) {
		std::wcout << L"ERRRRRROR\n";
		//TODO: raise an exception.
	}
	
	bool is_deleted = DeleteFileW(full_target_path);
	if (!is_deleted) {
		std::wcout << L"Failed Deleting " << full_target_path << L"\n";
		// Critical error. Nothing to do.
	}

	std::wcout << L"Deleting " << full_target_path << L"\n";
}

void FileInstaller::delete_installed_files() {

	// std::for_each(std::begin(this->file_paths), std::end(this->file_paths), this->copy_file);
	for (auto file_path = std::begin(this->file_paths_to_clean); file_path != std::end(this->file_paths_to_clean); ++file_path) {
		this->delete_file(*file_path);
	}
}

void FileInstaller::create_installation_dir() {
	if (!CreateDirectoryW(this->installation_dir, NULL)) {
		if (GetLastError() == ERROR_ALREADY_EXISTS) {
			this->is_dir_already_exists = true;
		}
		else {
			//TODO: raise here an exception. because we could make the dir.
		}
	}
}

void FileInstaller::delete_installation_dir() {
	if (this->is_dir_already_exists) {
		return;
	}

	if (!RemoveDirectoryW(this->installation_dir)) {
		//Critical error. Nothing to do.
	}

	std::wcout << L"Deleting Dir " << this->installation_dir << L"\n";
}

void FileInstaller::install() {
	this->create_installation_dir();
	this->copy_files();
}

FileInstaller::FileInstaller(std::vector<LPCWSTR> &file_paths, LPCWSTR installation_dir) : file_paths(file_paths) {
	this->installation_dir = installation_dir;
	this->is_dir_already_exists = false;
	this->file_paths_to_clean = {};
}

FileInstaller::~FileInstaller() {
	this->delete_installed_files();
	this->delete_installation_dir();
}

int main()
{
	FileInstaller f = FileInstaller(installation_files, installation_dir);
	f.install();
}