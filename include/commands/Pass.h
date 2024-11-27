#pragma once

#include "Command.h"

class Pass: public Command
{
public:
	Pass();
	~Pass();

	void execute(Server& server, Client& client, const std::vector<std::string>& args) const;
};