#pragma once

#include "Command.h"

class Ping: public Command
{
public:
	Ping();
	~Ping();

	void execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const;
};