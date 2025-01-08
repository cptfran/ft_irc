#pragma once

#include "Command.h"

class Who: public Command
{
public:
	Who();
	~Who();

	void execute(Server& server, Client& client, const std::vector<std::string>& args) const;
	
private:
	void handleChannel(Server& server, const std::string& mask, Client& requester, bool operatorOnly) const;
	void handleUsers(Server& server, Client& requester, const std::string& mask) const;
	bool userMatchesMask(Server& server, Client& requester, const std::string& mask) const;
	bool matchWildcard(const char* pattern, const char* str) const;
};