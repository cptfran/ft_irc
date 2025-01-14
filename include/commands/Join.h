#pragma once

#include "channel/Channel.h"
#include "Command.h"

class Join: public Command
{
public:
	Join();
	~Join();

	void execute(Server& server, Client& requester, const std::vector<std::string>& args) const;

private:
	Channel* findOrCreateChannel(Server& server, const std::string& channelName) const;
	bool isValidChannelKey(const std::vector<std::string>& args, const std::string& channelKey) const;
	void joinChannel(Client& requester, Channel& channelToJoin, const std::string& serverName) const;
};