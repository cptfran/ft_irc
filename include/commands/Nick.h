#pragma once

#include "Command.h"

class Nick: public Command
{
public:
	Nick();
	~Nick();

	void execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const;
};