#pragma once
#include <string>
#include <vector>
#include <map>
#include "Server.h"
#include "Client.h"


// TODO: delete this class and move fetchCommands() to server class?
class ClientTranslator
{
public:
	explicit ClientTranslator(const std::string& buffer);
	~ClientTranslator();

	std::string getBuffer() const;
	std::map<std::string, std::vector<std::string> > getCommands() const;

	// void fetchCommands(const Server& server, const Client& client);
	void fetchCommands();

private:
	ClientTranslator();

	std::string buffer;
	std::map<std::string, std::vector<std::string> > commands;
};
