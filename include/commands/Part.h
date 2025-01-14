#pragma once

#include "Command.h"

class Part: public Command
{
public:
	Part();
	~Part();

	void execute(Server& server, Client& requester, const std::vector<std::string>& args) const;
};