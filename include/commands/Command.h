#pragma once

#include "client/Client.h"
#include <string>
#include <vector>

class Server;
class Channel;

class Command
{
public:
	Command();
	virtual ~Command();

	virtual void execute(Server& server, Client& requester, const std::vector<std::string>& args) const = 0;

protected:
	void sendTopic(const Channel& channel, const Client& requester, const std::string& serverName) const;

private:
	Command(const Command& toCopy);
	Command& operator=(const Command& toAssign);
};