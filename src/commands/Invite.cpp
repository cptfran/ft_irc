#include "commands/Invite.h"
#include "server/Log.h"
#include "server/Server.h"
#include "utils/Utils.h"
#include "replier/Replier.h"

Invite::Invite()
{

}

Invite::~Invite()
{

}

void Invite::execute(Server& server, Client& requester, const std::vector<std::string>& args) const
{
    // Not enough parameters provided.
    if (args.size() < 2)
    {
        Replier::reply(requester.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(server.getName(),
            requester.getNickname(), std::string("INVITE")));
        return;
    }

    // Find the user which will be invited.
    const std::string& nicknameToInvite = args[0];
    const Client* clientToInvite = server.findClientByNickname(nicknameToInvite);

    // Invited user not found.
    if (clientToInvite == NULL)
    {
        Replier::reply(requester.getFd(), Replier::errNoSuchNick, Utils::anyToVec(server.getName(),
            requester.getNickname(), nicknameToInvite));
        return;
    }

    // Find the invitation channel.
    const std::string& channelToInviteName = args[1];
    Channel* channelToInvite = server.getChannel(channelToInviteName);

    // Invitation channel not found.
    if (channelToInvite == NULL)
    {
        Replier::reply(requester.getFd(), Replier::rplInviting, Utils::anyToVec(server.getName(),
            requester.getNickname(), clientToInvite->getNickname(), channelToInviteName));
        Replier::reply(requester.getFd(), Replier::rplInvite,
            Utils::anyToVec(requester.getNickname(), requester.getNickname(), channelToInviteName));
        return;
    }

    // Inviting client is not a member of the channel.
    if (!channelToInvite->isUserOnChannel(requester.getNickname()))
    {
        Replier::reply(requester.getFd(), Replier::errNotOnChannel, Utils::anyToVec(server.getName(),
            requester.getNickname(), channelToInviteName));
        return;
    }

    // User to invite is already on the channel.
    if (channelToInvite->isUserOnChannel(clientToInvite->getNickname()))
    {
        Replier::reply(requester.getFd(), Replier::errUserOnChannel, Utils::anyToVec(server.getName(),
            requester.getNickname(), nicknameToInvite, channelToInviteName));
        return;
    }

    // Channel is invite-only and inviting client doesn't have operator privileges.
    if (channelToInvite->isInviteOnly() && !channelToInvite->isUserOperator(requester.getNickname()))
    {
        Replier::reply(requester.getFd(), Replier::errChanOPrivsNeeded, Utils::anyToVec(server.getName(),
            requester.getNickname(), channelToInviteName));
        return;
    }

    // Add invited user nickname to the invited list of the channel and send invite replies.
    inviteUser(*channelToInvite, requester, *clientToInvite, server.getName());
}

void Invite::inviteUser(Channel& channelToInvite, const Client& invitingClient, const Client& invitedClient,
    const std::string& serverName) const
{
    // Add invited user nickname to the invited list of the channel.
    channelToInvite.addToInviteList(invitedClient.getNickname());

    // Send invite replies.
    Replier::reply(invitingClient.getFd(), Replier::rplInviting, Utils::anyToVec(serverName,
        invitingClient.getNickname(), invitedClient.getNickname(), channelToInvite.getName()));
    Replier::reply(invitedClient.getFd(), Replier::rplInvite,
        Utils::anyToVec(invitingClient.getNickname(), invitedClient.getNickname(), channelToInvite.getName()));
}