#pragma once

#include "Command.h"

class Ping: public Command
{
public:
	Ping();
	~Ping();

	void execute(const Server& server, Client& client, const std::vector<std::string>& args) const;
};