#pragma once

#include "Command.h"

class Who: public Command
{
public:
	Who();
	~Who();

	void execute(Server& server, Client& client, const std::vector<std::string>& args) const;
};