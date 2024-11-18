#pragma once

#include "../Client.h"
#include <string>
#include <vector>

class Server;

class Command
{
public:
	Command();
	virtual ~Command();

	virtual void execute(const Server& server, Client& client, const std::vector<std::string>& args) const = 0;
};