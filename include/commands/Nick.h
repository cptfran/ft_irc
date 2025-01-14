#pragma once

#include "Command.h"

class Nick: public Command
{
public:
	Nick();
	~Nick();

	void execute(Server& server, Client& requester, const std::vector<std::string>& args) const;
};