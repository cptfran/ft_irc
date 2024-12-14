#include "commands/Kick.h"
#include "Log.h"
#include "Server.h"
#include "Utils.h"
#include "Replier.h"

Kick::Kick()
{

}

Kick::~Kick()
{

}

// TODO: infinite loop happening when client is kicked by another client and then tries to connect again to the same
// channel.
void Kick::execute(Server& server, Client& client, const std::vector<std::string>& args) const
{
    const std::string& serverName = server.getName();
    const int clientFd = client.getFd();

    if (args.size() < 2)
    {
        const std::string command = "KICK";
        Replier::reply(clientFd, Replier::errNeedMoreParams, Utils::anyToVec(serverName, command));
        return;
    }

    const std::string& channelName = args[0];
    Channel* channel;

    // Fetch channel from server's channel list.
    try
    {
        channel = &server.findChannel(channelName);
    }
    // Channel not found.
    catch (const std::exception&)
    {
        Replier::reply(clientFd, Replier::errNoSuchChannel, Utils::anyToVec(serverName, channelName));
        return;
    }

    Channel::ClientData* clientData;

    // Fetch client data from channel's client list.
    try
    {
        clientData = &channel->findClientData(client);
    }
    // Client not on the channel->
    catch (const std::exception&)
    {
        Replier::reply(clientFd, Replier::errNotOnChannel, Utils::anyToVec(serverName, channelName));
        return;
    }

    // Client is not an operator.
    if (!clientData->isOperator)
    {
        Replier::reply(clientFd, Replier::errChanOPrivsNeeded, Utils::anyToVec(serverName, channelName));
        return;
    }

    const std::string& userToKick = args[1];

    // Get fds list for broadcast message before client is kicked, so it will receive kick information.
    const std::vector<int>& clientsFdList = channel->getFdsList();

    // Kick client from the channel->
    if (!channel->ejectClient(userToKick))
    {
        Log::msgServer(INFO, "CLIENT", clientFd, EJECT_CLIENT_FAIL);
        return;
    }

    // Broadcast reply after kick.
    std::vector<std::string> rplKickArgs = Utils::anyToVec(client.getNickname(), userToKick, channelName);
    if (args.size() == 3)
    {
        const std::string& comment = args[2];
        rplKickArgs.push_back(comment);
    }
    Replier::broadcast(clientsFdList, Replier::rplKick, rplKickArgs);
}
