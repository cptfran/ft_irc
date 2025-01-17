#include "manager/Config.h"

Config::Config(const std::string& name, const std::string& version, const std::string& password) : 
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

Config::~Config()
{

}

void Config::setVersion(const std::string& version)
{
	this->version = version;
}

std::string Config::getAvailableUserModes() const
{
	return this->availableUserModes;
}

std::string Config::getName() const
{
	return this->name;
}

std::string Config::getPassword() const
{
	return this->password;
}