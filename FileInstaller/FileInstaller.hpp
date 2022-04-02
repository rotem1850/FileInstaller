#pragma once

#include <vector>

#ifdef _DEBUG
#define DEBUG_MSG(str) do { std::wcout << L"[" << __FUNCTION__ << "] " << str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif

enum FileInstallerStatus : uint32_t {
	FILEINSTALLER_SUCCESS = 0,
	

	FILEUTILS_COPY_FILE_COPY_FAILED,
	FILEUTILS_CHANGE_FILE_PATH_DIRECTORY_INVALID_ARGUMENT,
	FILEUTILS_DELETE_FILE_DELETE_FAILED,
	FILEUTILS_CREATE_DIRECTORY_CREATE_FAILED,
	FILEUTILS_DELETE_DIRECTORY_REMOVE_FAILED,
	FILEUTILS_CHANGE_FILE_PATH_DIRECTORY_JOIN_PATH_FAILED,

};

class FileInstallerException : public std::exception
{
private:
	FileInstallerStatus status;

public:
	FileInstallerException(FileInstallerStatus status) : status(status) {};
	FileInstallerStatus get_status() { return status; }
};

/*
FileInstaller - Allows installation of files in a target directory.

Notice: FileInstaller class doesn't support paths longer than MAX_PATH.
*/
class FileInstaller final {
public:
	FileInstaller(std::vector<std::wstring> &file_paths, std::wstring installation_dir);
	virtual ~FileInstaller();
	void install();

private:
	void _copy_file(std::wstring const &file_path);
	void _delete_file(std::wstring const &file_path);
	void _copy_files();
	void _create_installation_dir();
	
	//This method is working by best effort.
	FileInstallerStatus _delete_installation_dir();

	//This method is working by best effort.
	FileInstallerStatus _delete_installed_files();

	/*
	The revert method cant be called standalone because it will revert only things performed during the install method.
	This is working by best effort.
	*/
	void _revert_installation();

private:
	std::vector<std::wstring> &m_file_paths;
	std::wstring m_installation_dir;
	bool m_is_dir_already_exists;
	std::vector<std::wstring> m_file_paths_to_clean;


};