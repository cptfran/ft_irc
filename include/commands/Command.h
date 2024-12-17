#pragma once

#include "../Client.h"
#include "Channel.h"
#include <string>
#include <vector>

class Server;

class Command
{
public:
	Command();
	virtual ~Command();

	virtual void execute(Server& server, Client& client, const std::vector<std::string>& args) const = 0;

protected:
	void handleMissingParams(const std::string& command, int clientFd, const std::string& serverName) const;
	Channel* findChannel(Server& server, const std::string& channelToFindName) const;
	virtual void handleChannelNotFound(const Client& requestingClient, const std::string& serverName,
		const Client& invitedClient, const std::string& channelName) const;
	void handleUserNotOnChannel(int requestingFd, const std::string& serverName,
		const std::string& channelName) const;
	void handleNoOperatorPriv(int requestingFd, const std::string& serverName,
		const std::string& channelName) const;
	void handleUserNotInChannel(int requestingFd, const std::string& serverName, const std::string& nickname,
		const std::string& channelName) const;
};