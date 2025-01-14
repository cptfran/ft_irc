#pragma once
#include <string>
#include <vector>
#include <map>
#include "server/Server.h"
#include "client/Client.h"

#define CHANNEL_NAME_MAX_LENGTH 50

class ClientTranslator
{
public:
	~ClientTranslator();

	static std::vector<std::pair<std::string, std::vector<std::string> > > fetchCommands(const std::string& buffer,
		const std::map<std::string, Command*>& validServerCommands);
	static bool nicknameValid(const std::string& nickname);
	static std::vector<std::string> splitToTokens(const std::string& str, const char splitter);
	static bool matchWildcard(const char* pattern, const char* str);
	static std::string sanitizeColonMessage(const std::string& message);
	static std::string sanitizeChannelName(const std::string& name);
	static void parseUserTarget(const std::string& extrTarget, std::string& nickname, std::string& username,
		std::string& hostname, std::string& serverName);
	static void assignUserInfo(const std::string extrTarget, std::vector<char>::iterator splitter,
		std::vector<char>::iterator splittersEnd, std::string& nickname, std::string& username, std::string& hostname,
		std::string& serverName);
private:
	ClientTranslator();
};
