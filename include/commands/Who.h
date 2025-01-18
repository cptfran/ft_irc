#pragma once

#include "Command.h"

class Who: public Command
{
public:
	Who();
	~Who();

	void execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const;
	
private:
	void handleChannel(Manager& serverManager, const std::string& mask, Client& requester, bool operatorOnly) const;
	void handleUsers(Manager& serverManager, Client& requester, const std::string& mask) const;
	bool userMatchesMask(const std::string& serverName, Client& requester, const std::string& mask) const;
};