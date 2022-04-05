#include <windows.h>
#include <iostream>
#include <vector>

#include "FileInstaller.hpp"

Installer::Installer(std::vector<std::shared_ptr<ResourceInstaller>> &resource_installers)
	:m_resource_installers(resource_installers),
	m_installed_resource_installers({}),
	m_is_installed(false) {

	// Validate input doen't have any null values. So there will be no surprises during installation.
	for (auto& resource_installer : this->m_resource_installers) {
		if (nullptr == resource_installer) {
			throw FileInstallerException(FileInstallerStatus::INSTALLER_INVALID_ARGUMENTS);
		}
	}
}

Installer::~Installer() {
	if (!this->m_is_installed) {
		this->_revert();
	}
}

void Installer::install() {
	for (auto& resource_installer : this->m_resource_installers) {
		resource_installer->install();
		this->m_installed_resource_installers.push_back(resource_installer);
	}

	this->m_is_installed = true;
}

void Installer::_revert() {
	DEBUG_MSG("Reverting installation (best effort)");
	// We revert in reverse order so the files will be deleted first and then the directories.
	for (auto resource_installer = this->m_installed_resource_installers.rbegin(); resource_installer != this->m_installed_resource_installers.rend(); ++resource_installer)
	{
		try {
			(*resource_installer)->revert();
		}
		catch (FileInstallerException) {
			// Best effort
		}
		
	}
}
