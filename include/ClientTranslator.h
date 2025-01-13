#pragma once
#include <string>
#include <vector>
#include <map>
#include "Server.h"
#include "Client.h"

class ClientTranslator
{
public:
	~ClientTranslator();

	static std::map<std::string, std::vector<std::string> > fetchCommands(const std::string& buffer,
		const std::map<std::string, Command*>& validServerCommands);
	static std::vector<std::string> splitToTokens(const std::string& str, const char splitter);
	static bool matchWildcard(const char* pattern, const char* str);
	static std::string sanitizeColonMessage(const std::string& message);
private:
	ClientTranslator();
};
