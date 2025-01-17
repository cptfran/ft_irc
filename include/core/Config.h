#pragma once

#include <string>

class Config
{
public:
	Config(const std::string& name, const std::string& version, const std::string& password);
	~Config();

	void setVersion(const std::string& version);

	std::string getAvailableUserModes() const;
	std::string getName() const;
	std::string getPassword() const;

private:
	int fd;
	std::string name;
	std::string version;
	std::string password;
	std::string creationDate;
	std::string availableUserModes;
	std::string availableChannelModes;

	Config();
	Config(const Config& toCopy);
	Config& operator=(const Config& toAssign);
};