#include <windows.h>
#include <iostream>
#include <vector>

#include "FileInstaller.hpp"

Installer::Installer(std::vector<std::shared_ptr<ResourceInstaller>> &resource_installers)
	:m_resource_installers(resource_installers),
	m_installed_resource_installers(),
	m_is_installed(std::make_shared<bool>(false)) {

	// Validate input doen't have any null values. So there will be no surprises during installation.
	for (auto& resource_installer : this->m_resource_installers) {
		if (nullptr == resource_installer) {
			throw FileInstallerException(FileInstallerStatus::INSTALLER_INVALID_ARGUMENTS);
		}
	}
}

Installer::~Installer() {}

void Installer::install() {
	for (auto& resource_installer : this->m_resource_installers) {
		auto safe_resource_installer = std::make_shared<Installer::SafeResourceInstaller>(this->m_is_installed, resource_installer);
		safe_resource_installer->install();

		// We push the installers to a queue so the destructors will be called in reverse order.
		this->m_installed_resource_installers.push(safe_resource_installer);
	}

	*this->m_is_installed = true;
}
