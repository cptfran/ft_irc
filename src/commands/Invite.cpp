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
    const std::string& serverName = server.getName();
    const int clientFd = client.getFd();

    // Not enought parameters provided.
    if (args.size() < 2)
    {
        const std::string command = "INVITE";
        Replier::reply(clientFd, Replier::errNeedMoreParams, Utils::anyToVec(serverName, command));
        return;
    }

    const std::string& nickname = args[0];
    Client* clientToInvite;

    // Find the client which will be invited.
    try
    {
        clientToInvite = &server.findClientByNickname(nickname);
    }
    catch (const std::exception&)
    {
        Replier::reply(clientFd, Replier::errNoSuchNick, Utils::anyToVec(serverName, nickname));
        return;
    }

    const std::string& channelToInviteName = args[1];
    const std::string& clientToInviteNickname = clientToInvite->getNickname();
    const int clientToInviteFd = clientToInvite->getFd();
    const std::string& clientInvitingNickname = client.getNickname();
    Channel* channelToInvite;

    // Find channel to invite. If there is no such channel, still send invitation.
    try
    {
        channelToInvite = &server.findChannel(channelToInviteName);
    }
    catch (const std::exception&)
    {
        Replier::reply(clientFd, Replier::rplInviting,
            Utils::anyToVec(serverName, clientToInviteNickname, channelToInviteName));
        Replier::reply(clientToInviteFd, Replier::rplInvite,
            Utils::anyToVec(clientInvitingNickname, clientToInviteNickname, channelToInviteName));
        return;
    }

    Channel::ClientData* invitingClientData;

    // Try to fetch inviting client data from the channel.
    try
    {
        invitingClientData = &channelToInvite->findClientData(client);
    }
    // Inviting client is not a member of the channel.
    catch (const std::exception&)
    {
        Replier::reply(clientFd, Replier::errNotOnChannel, Utils::anyToVec(serverName, channelToInviteName));
        return;
    }

    // User to invite is already on the channel.
    if (channelToInvite->isClientOnChannel(clientToInviteNickname))
    {
        Replier::reply(clientFd, Replier::errUserOnChannel,
            Utils::anyToVec(serverName, clientToInviteNickname, channelToInviteName));
        return;
    }

    // Channel is invite-only and inviting client doesn't have operator privileges.
    if (channelToInvite->isInviteOnly() && !invitingClientData->isOperator)
    {
        Replier::reply(clientFd, Replier::errChanOPrivsNeeded,  Utils::anyToVec(serverName, channelToInviteName));
        return;
    }

    // Add invited user nickname to the invited list of the channel.
    channelToInvite->addToInviteList(clientToInviteNickname);

    // Send invite replies.
    Replier::reply(clientFd, Replier::rplInviting,
            Utils::anyToVec(serverName, clientToInviteNickname, channelToInviteName));
    Replier::reply(clientToInviteFd, Replier::rplInvite,
        Utils::anyToVec(clientInvitingNickname, clientToInviteNickname, channelToInviteName));
}
