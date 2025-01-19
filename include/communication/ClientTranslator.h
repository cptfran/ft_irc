#pragma once
#include <string>
#include <vector>
#include <map>
#include "core/Server.h"
#include "data/Client.h"

class ClientTranslator
{
public:
	~ClientTranslator();

	static std::string parseClientBufferFromRecv(int fd);
	static std::pair<std::string, std::vector<std::string> > fetchCmdAndArgs(const std::string& buffer);
	static bool nicknameValid(const std::string& nickname);
	static std::vector<std::string> splitToTokens(const std::string& str, const char splitter);
	static bool matchWildcard(const char* pattern, const char* str);
	static std::string sanitizeColonMessage(const std::string& message);
	static std::string sanitizeChannelName(const std::string& name);
	static void parseUserTarget(const std::string& extrTarget, std::string& nickname, std::string& username,
		std::string& hostname, std::string& serverName);
	static void assignUserInfo(std::string extrTarget, std::vector<char>::iterator splitter,
		std::vector<char>::iterator splittersEnd, std::string& nickname, std::string& username, std::string& hostname,
		std::string& serverName);
private:
	ClientTranslator();
	ClientTranslator(const ClientTranslator& toCopy);
	ClientTranslator& operator=(const ClientTranslator& toAssign);
};
