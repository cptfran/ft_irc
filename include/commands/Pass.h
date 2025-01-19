#pragma once

#include "Command.h"

class Pass: public Command
{
public:
	Pass();
	~Pass();

	void execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const;
};