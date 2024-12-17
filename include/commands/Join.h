#pragma once

#include "Channel.h"
#include "Command.h"

class Join: public Command
{
public:
	Join();
	~Join();

	void execute(Server& server, Client& client, const std::vector<std::string>& args) const;

private:
	void handleNotRegistered(int clientFd, const std::string& serverName) const;
	void handleTooManyChannels(int clientFd, const std::string& serverName, const std::string& channelName) const;
	Channel* findOrCreateChannel(Server& server, const std::string& channelName) const;
	void handleInviteOnly(int clientFd, const std::string& serverName, const std::string& channelName) const;
	bool isValidChannelKey(const std::vector<std::string>& args, const std::string& channelKey) const;
	void handleInvalidChannelKey(int clientFd, const std::string& serverName, const std::string& channelName) const;
	void joinChannel(Client& client, Channel* channelToJoin, const std::string& serverName) const;
};