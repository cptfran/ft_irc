#pragma once
#include <string>
#include <vector>
#include <map>
#include "Server.h"
#include "Client.h"


// : delete this class and move fetchCommands() to server class?
// If this will be used only in Server class yes, otherwise maybe leave it like this.
class ClientTranslator
{
public:
	~ClientTranslator();

	std::string getBuffer() const;

	static std::map<std::string, std::vector<std::string> > fetchCommands(const std::string& buffer,
		const std::map<std::string, Command*>& validServerCommands);

private:
	ClientTranslator();
};
