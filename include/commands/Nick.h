#pragma once

#include "Command.h"

class Nick: public Command
{
public:
	Nick();
	~Nick();

	void execute(const Server& server, Client& client, const std::vector<std::string>& args) const;
};