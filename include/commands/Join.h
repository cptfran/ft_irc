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
	Channel* findOrCreateChannel(Server& server, const std::string& channelName) const;
	bool isValidChannelKey(const std::vector<std::string>& args, const std::string& channelKey) const;
	void joinChannel(Client& client, Channel& channelToJoin, const std::string& serverName) const;
};