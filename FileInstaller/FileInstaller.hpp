#pragma once

#include <vector>
#include <queue>  

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

/*
* An abstract class used for adding new operation to the install process.
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

/*
Installer - Used for perform installation using sequence of ResourceInstallers.

Notice: Installer class doesn't support paths longer than MAX_PATH.
*/
class Installer final {
public:
	// resource_installers is a vector of ResourceInstallers to be performed during installation.
	Installer(std::vector<std::shared_ptr<ResourceInstaller>>& resource_installers);
	virtual ~Installer();
	void install();

	Installer(const Installer&) = delete;
	Installer& operator=(Installer const&) = delete;

private:
	// This inner class is responsible for all ResourceInstallers revertion in case of failure.
	class SafeResourceInstaller final {
	public:
		SafeResourceInstaller(std::shared_ptr<bool> is_installed, std::shared_ptr<ResourceInstaller> resource_installer) : m_is_installed(is_installed), m_resource_installer(resource_installer) {};
		virtual ~SafeResourceInstaller() {
			if (!(*m_is_installed)) {
				this->m_resource_installer->revert();
			}
		};

		void install() {
			this->m_resource_installer->install();
		};

		SafeResourceInstaller(const SafeResourceInstaller&) = delete;
		SafeResourceInstaller& operator=(SafeResourceInstaller const&) = delete;

	private:
		std::shared_ptr<ResourceInstaller> m_resource_installer;
		// This is a shared_ptr cause its shared with the inner class.
		std::shared_ptr<bool> m_is_installed;
	};

private:
	std::vector<std::shared_ptr<ResourceInstaller>> m_resource_installers;
	std::queue<std::shared_ptr<SafeResourceInstaller>> m_installed_resource_installers;
	std::shared_ptr<bool> m_is_installed;

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