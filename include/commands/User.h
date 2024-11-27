#pragma once

#include "Command.h"

class User: public Command
{
public:
	User();
	~User();

	void execute(Server& server, Client& client, const std::vector<std::string>& args) const;
};