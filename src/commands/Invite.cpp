#include "commands/Invite.h"
#include "Log.h"
#include "Server.h"
#include "Utils.h"
#include "Replier.h"

Invite::Invite()
{

}

Invite::~Invite()
{

}

void Invite::execute(Server& server, Client& client, const std::vector<std::string>& args) const
{

    // Not enough parameters provided.
    if (args.size() < 2)
    {
        handleMissingParams("INVITE", client.getFd(), server.getName());
        return;
    }

    // Find the user which will be invited.
    const std::string& nicknameToInvite = args[0];
    const Client* clientToInvite = findInvitedUser(server, nicknameToInvite);

    // Invited user not found.
    if (clientToInvite == NULL)
    {
        handleInvitedUserNotFound(client.getFd(), server.getName(), nicknameToInvite);
        return;
    }

    // Find the invitation channel.
    const std::string& channelToInviteName = args[1];
    Channel* channelToInvite = findChannel(server, channelToInviteName);

    // Invitation channel not found.
    if (channelToInvite == NULL)
    {
        handleChannelNotFound(client, server.getName(), *clientToInvite,
            channelToInviteName);
        return;
    }

    // Inviting client is not a member of the channel.
    if (!channelToInvite->isUserOnChannel(client.getNickname()))
    {
        handleUserNotOnChannel(client.getFd(), server.getName(),channelToInviteName);
        return;
    }

    // User to invite is already on the channel.
    if (channelToInvite->isUserOnChannel(clientToInvite->getNickname()))
    {
        handleUserAlreadyOnChannel(client.getFd(), server.getName(), nicknameToInvite, channelToInviteName);
        return;
    }

    // Channel is invite-only and inviting client doesn't have operator privileges.
    if (channelToInvite->isInviteOnly() && !channelToInvite->isUserOperator(client.getNickname()))
    {
        handleNoOperatorPriv(client.getFd(), server.getName(), channelToInviteName);
        return;
    }

    // Add invited user nickname to the invited list of the channel and send invite replies.
    inviteUser(*channelToInvite, client, *clientToInvite, server.getName());
}

Client* Invite::findInvitedUser(Server& server, const std::string& nicknameToFind) const
{
    try
    {
        return &server.findClientByNickname(nicknameToFind);
    }
    catch (const std::exception&)
    {
        return NULL;
    }
}

void Invite::handleInvitedUserNotFound(const int invitingFd, const std::string& serverName,
    const std::string& nicknameToFind) const
{
    Replier::reply(invitingFd, Replier::errNoSuchNick, Utils::anyToVec(serverName, nicknameToFind));
}

void Invite::handleChannelNotFound(const Client& requestingClient, const std::string& serverName,
    const Client& invitedClient, const std::string& channelName) const
{
    Replier::reply(invitingClient.getFd(), Replier::rplInviting,
            Utils::anyToVec(serverName, invitedClient.getNickname(), channelToInviteName));
    Replier::reply(invitedClient.getFd(), Replier::rplInvite,
        Utils::anyToVec(invitingClient.getNickname(), invitedClient.getNickname(), channelToInviteName));
}

void Invite::handleUserAlreadyOnChannel(const int invitingFd, const std::string& serverName,
    const std::string& invitedNickname, const std::string& channelToInviteName) const
{
    Replier::reply(invitingFd, Replier::errUserOnChannel,
           Utils::anyToVec(serverName, invitedNickname, channelToInviteName));
}

void Invite::inviteUser(Channel& channelToInvite, const Client& invitingClient, const Client& invitedClient,
    const std::string& serverName) const
{
    // Add invited user nickname to the invited list of the channel.
    channelToInvite.addToInviteList(invitedClient.getNickname());

    // Send invite replies.
    Replier::reply(invitingClient.getFd(), Replier::rplInviting,
            Utils::anyToVec(serverName, invitedClient.getNickname(), channelToInvite.getName()));
    Replier::reply(invitedClient.getFd(), Replier::rplInvite,
        Utils::anyToVec(invitingClient.getNickname(), invitedClient.getNickname(), channelToInvite.getName()));
}