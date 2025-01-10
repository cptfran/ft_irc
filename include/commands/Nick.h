#pragma once

#include "Command.h"

// TODO: implement nick change.
class Nick: public Command
{
public:
	Nick();
	~Nick();

	void execute(Server& server, Client& client, const std::vector<std::string>& args) const;
};