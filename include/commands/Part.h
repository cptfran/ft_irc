#pragma once

#include "Command.h"

class Part: public Command
{
public:
	Part();
	~Part();

	void execute(Server& server, Client& client, const std::vector<std::string>& args) const;
};