#pragma once

#include "Command.h"

class Cap: public Command
{
public:
	Cap();
	~Cap();

	void execute(const Server& server, Client& client, const std::vector<std::string>& args) const;
};