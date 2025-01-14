#pragma once

#include "Command.h"

class Cap: public Command
{
public:
	Cap();
	~Cap();

	void execute(Server& server, Client& requester, const std::vector<std::string>& args) const;
};