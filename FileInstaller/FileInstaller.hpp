#pragma once

#include <vector>

#ifdef _DEBUG
#define DEBUG_MSG(str) do { std::wcout << L"[" << __FUNCTION__ << "] " << str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif

enum FileInstallerStatus : uint32_t {
	FILEINSTALLER_SUCCESS = 0,
	FILEINSTALLER_COPY_FILE_JOIN_PATH_FAILED,
	FILEINSTALLER_COPY_FILE_COPY_FAILED,
	FILEINSTALLER_DELETE_FILE_JOIN_PATH_FAILED,
	FILEINSTALLER_DELETE_FILE_DELETE_FAILED,
	FILEINSTALLER_CREATE_INSTALLATION_DIR_CREATE_FAILED,
	FILEINSTALLER_DELETE_INSTALLATION_DIR_REMOVE_FAILED,
};

class FileInstallerException : public std::exception
{
private:
	FileInstallerStatus status;

public:
	FileInstallerException(FileInstallerStatus status) :status(status) {};
	FileInstallerStatus get_status() { return status; }
};

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
	
	/* The revert method cant be called standalone because it will revert only things performed during the install method.*/
	void revert_installation();

};