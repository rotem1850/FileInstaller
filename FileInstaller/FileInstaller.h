#pragma once
#include <windows.h>
#include <vector>

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