#include "manager/ConfigManager.h"
#include <ctime>
#include <unistd.h>

ConfigManager::ConfigManager(const std::string& name, const std::string& version, const std::string& password) : 
	fd(-1), name(name), version(version), password(password), availableUserModes(std::string("i")),
	availableChannelModes(std::string("itkol"))
{
	const std::time_t now = std::time(NULL);
	creationDate = std::ctime(&now);
	if (creationDate[creationDate.size() - 1] == '\n')
	{
		creationDate.resize(creationDate.size() - 1);
	}

}

ConfigManager::~ConfigManager()
{
	if (this->fd != -1)
	{
		close(this->fd);
	}
}

void ConfigManager::setFd(const int fd)
{
	this->fd = fd;
}

void ConfigManager::setVersion(const std::string& version)
{
	this->version = version;
}

int ConfigManager::getFd() const
{
	return this->fd;
}

const std::string& ConfigManager::getName() const
{
	return this->name;
}

const std::string& ConfigManager::getVersion() const
{
	return this->version;
}

const std::string& ConfigManager::getPassword() const
{
	return this->password;
}

const std::string& ConfigManager::getCreationDate() const
{
	return this->creationDate;
}

const std::string& ConfigManager::getAvailableUserModes() const
{
	return this->availableUserModes;
}

const std::string& ConfigManager::getAvailableChannelModes() const
{
	return this->availableChannelModes;
}
