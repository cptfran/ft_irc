#pragma once

#include "Command.h"

class Part: public Command
{
public:
	Part();
	~Part();

	void execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const;
};