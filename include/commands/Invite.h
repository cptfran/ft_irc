#pragma once

#include "Channel.h"
#include "Command.h"

class Invite: public Command
{
public:
    Invite();
    ~Invite();

    void execute(Server& server, Client& client, const std::vector<std::string>& args) const;
	void inviteUser(Channel& channelToInvite, const Client& invitingClient, const Client& invitedClient,
		const std::string& serverName) const;
};

