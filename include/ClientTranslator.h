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
	explicit ClientTranslator(const std::string& buffer);
	~ClientTranslator();

	std::string getBuffer() const;
	std::map<std::string, std::vector<std::string> > getCommands() const;

	void fetchCommands(std::vector<std::string> commandList);

private:
	ClientTranslator();

	std::string buffer;
	std::map<std::string, std::vector<std::string> > commands;
};
