#pragma once

#include "Command.h"

class User: public Command
{
public:
	User();
	~User();

	void execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const;
};