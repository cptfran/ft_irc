#pragma once

#include "Channel.h"
#include "Command.h"

class Invite: public Command
{
public:
    Invite();
    ~Invite();

    void execute(Server& server, Client& client, const std::vector<std::string>& args) const;
    Client* findInvitedUser(Server& server, const std::string& nicknameToFind) const;
    void handleInvitedUserNotFound(int invitingFd, const std::string& serverName,
        const std::string& nicknameToFind) const;
	void handleChannelNotFound(const Client& requestingClient, const std::string& serverName,
		const Client& invitedClient, const std::string& channelName) const;
	void handleUserAlreadyOnChannel(int invitingFd, const std::string& serverName,	const std::string& invitedNickname,
		const std::string& channelToInviteName) const;
	void inviteUser(Channel& channelToInvite, const Client& invitingClient, const Client& invitedClient,
		const std::string& serverName) const;
};

