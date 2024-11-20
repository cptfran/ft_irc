#pragma once

#include "Command.h"

class Join: public Command
{
public:
	Join();
	~Join();

	void execute(Server& server, Client& client, const std::vector<std::string>& args) const;
};