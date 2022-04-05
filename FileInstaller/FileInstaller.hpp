#pragma once

#include <vector>

#ifdef _DEBUG
#define DEBUG_MSG(str) do { std::wcout << L"[" << __FUNCTION__ << "] " << str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif

enum class FileInstallerStatus: uint32_t{
	FILEINSTALLER_SUCCESS = 0,

	INSTALLER_INVALID_ARGUMENTS,
	FILEUTILS_COPY_FILE_COPY_FAILED,
	FILEUTILS_DELETE_FILE_DELETE_FAILED,
	FILEUTILS_CREATE_DIRECTORY_CREATE_FAILED,
	FILEUTILS_DELETE_DIRECTORY_REMOVE_FAILED,
	FILEUTILS_IS_PATH_EXISTS_FAILED,
};

class FileInstallerException : public std::exception
{
public:
	FileInstallerException(FileInstallerStatus status) : status(status) {};
	FileInstallerStatus get_status() const { return status; }

private:
	FileInstallerStatus status;
};

class ResourceInstaller;

/*
Installer - Used for perform installation using sequence of ResourceInstallers.

Notice: Installer class doesn't support paths longer than MAX_PATH.
*/
class Installer final {
public:
	Installer(std::vector<std::shared_ptr<ResourceInstaller>>& resource_installers);
	virtual ~Installer();
	void install();

	Installer(const Installer&) = delete;
	Installer& operator=(Installer const&) = delete;

private:
	/*
	The revert method cant be called standalone because it will revert only things performed during the install method.
	This is working by best effort.
	*/
	void _revert();

private:
	std::vector<std::shared_ptr<ResourceInstaller>> m_resource_installers;
	std::vector<std::shared_ptr<ResourceInstaller>> m_installed_resource_installers;
	bool m_is_installed;
};

/*
* An abstract class used for adding new installation operation to the install process.
* For example:
*	- Operation of changing the registry property will derive from this class.
*/
class ResourceInstaller {
public:
	virtual void install() = 0;
	/*
	The revert method cant be called standalone because it will revert only things performed during the install method.
	This is working by best effort.
	*/
	virtual void revert() = 0;

};

// FileInstaller - Used for installation of a file (copy file to a target dir).
class FileInstaller final : public ResourceInstaller {
public:
	FileInstaller(std::wstring const &file_path, std::wstring const &installation_dir);
	virtual ~FileInstaller();
	virtual void install();
	virtual void revert();

	FileInstaller(const FileInstaller&) = delete;
	FileInstaller& operator=(FileInstaller const&) = delete;

private:
	void _copy_file();
	void _delete_file();

private:
	std::wstring m_file_path;
	std::wstring m_installation_dir;
	bool m_is_file_already_exists;
};

// DirInstaller - Used for installation of a directory (create new empty directory).
class DirInstaller final : public ResourceInstaller {
public:
	DirInstaller(std::wstring const &directory_path);
	virtual ~DirInstaller();
	virtual void install();
	virtual void revert();

	DirInstaller(const DirInstaller&) = delete;
	DirInstaller& operator=(DirInstaller const&) = delete;

private:
	void _create_directory();
	void _delete_directory();

private:
	std::wstring m_directory_path;
	bool m_is_directory_already_exists;
};