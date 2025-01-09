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
	static std::vector<std::string> extractPrivmsgTargets(const std::string& targets);
private:
	ClientTranslator();
};
