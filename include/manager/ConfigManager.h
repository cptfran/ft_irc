#pragma once

#include <string>

class ConfigManager
{
public:
	ConfigManager(const std::string& name, const std::string& version, const std::string& password);
	~ConfigManager();

	void setFd(int fd);
	void setVersion(const std::string& version);

	int getFd() const;
	std::string getName() const;
	std::string getVersion() const;
	std::string getPassword() const;
	std::string getCreationDate() const;
	std::string getAvailableUserModes() const;
	std::string getAvailableChannelModes() const;

private:
	int fd;
	std::string name;
	std::string version;
	std::string password;
	std::string creationDate;
	std::string availableUserModes;
	std::string availableChannelModes;

	ConfigManager();
	ConfigManager(const ConfigManager& toCopy);
	ConfigManager& operator=(const ConfigManager& toAssign);
};