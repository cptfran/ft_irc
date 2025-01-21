#pragma once

#include <string>

class ConfigManager
{
public:
	// Constants:
	static const int INPUT_BUFFER_SIZE = 512;
	static const int TIME_FOR_CLIENT_TO_REGISTER = 60;
	static const int REGISTERED_PORT_MIN = 1024;
	static const int REGISTERED_PORT_MAX = 49151;
	static const int MAX_PRIVMSG_TARGETS = 5;
	static const int MAX_CHANNELS_CLIENT_CAN_JOIN = 10;
	static const int MAX_USERNAME_LEN = 9;
	static const int CHANNEL_NAME_MAX_LENGTH = 50;


	ConfigManager(const std::string& name, const std::string& version, const std::string& password);
	~ConfigManager();

	void setFd(int fd);
	void setVersion(const std::string& version);

	int getFd() const;
	const std::string& getName() const;
	const std::string& getVersion() const;
	const std::string& getCreationDate() const;
	const std::string& getAvailableUserModes() const;
	const std::string& getAvailableChannelModes() const;

	bool passwordValidator(const std::string& pass) const;

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