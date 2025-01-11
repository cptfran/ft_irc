#pragma once

#include "../Client.h"
#include <string>
#include <vector>

class Server;
class Channel;

class Command
{
public:
	Command();
	virtual ~Command();

	virtual void execute(Server& server, Client& client, const std::vector<std::string>& args) const = 0;

protected:
	//virtual bool validateArgs(Server& server, Client& client, const std::vector<std::string>& args) const = 0;
	void sendTopic(const Channel& channel, const Client& requestor, const std::string& serverName) const;
};