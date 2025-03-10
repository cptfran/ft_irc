#pragma once

#include "data/Channel.h"
#include "Command.h"

class Invite: public Command
{
public:
    Invite();
    ~Invite();

    void execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const;
	void inviteUser(Channel& channelToInvite, const Client& invitingClient, const Client& invitedClient,
		const std::string& serverName) const;
};

