#pragma once

#include "Command.h"

#define MAX_USERNAME_LEN 9

class User: public Command
{
public:
	User();
	~User();

	void execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const;
};