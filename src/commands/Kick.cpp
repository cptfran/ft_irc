#include "commands/Kick.h"

#include "Channel.h"
#include "Server.h"
#include "Utils.h"
#include "Replier.h"

Kick::Kick()
{

}

Kick::~Kick()
{

}

void Kick::execute(Server& server, Client& client, const std::vector<std::string>& args) const
{
    // Not enough parameters provided.
    if (args.size() < 2)
    {
        Replier::reply(client.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(server.getName(),
            client.getNickname(), std::string("KICK")));
        return;
    }

    // Find the channel.
    const std::string& channelName = args[0];
    Channel* channel = server.getChannel(channelName);

    // Channel not found.
    if (channel == NULL)
    {
        Replier::reply(client.getFd(), Replier::errNoSuchChannel, Utils::anyToVec(server.getName(),
            client.getNickname(), channelName));
        return;
    }

    // Client not on the channel.
    if (!channel->isUserOnChannel(client.getNickname()))
    {
        Replier::reply(client.getFd(), Replier::errNotOnChannel, Utils::anyToVec(server.getName(),
            client.getNickname(), channelName));
        return;
    }

    // Client is not an operator.
    if (!channel->isUserOperator(client.getNickname()))
    {
        Replier::reply(client.getFd(), Replier::errChanOPrivsNeeded, Utils::anyToVec(server.getName(),
            client.getNickname(), channelName));
        return;
    }

    kickUser(args, *channel, client, server);
}

void Kick::kickUser(const std::vector<std::string>& args, Channel& channel, const Client& requestor,
    Server& server) const
{
    const std::string& userToKick = args[1];

    // Get fds list for broadcast message before client is kicked, so it will receive kick information.
    const std::vector<int>& clientsFdList = channel.getFdsList();

    // Kick user from the channel.
    if (!channel.ejectUser(server, userToKick))
    {
        Replier::reply(requestor.getFd(), Replier::errUserNotInChannel,
            Utils::anyToVec(server.getName(), requestor.getNickname(), userToKick, channel.getName()));
        return;
    }

    // Broadcast reply after kick.
    std::vector<std::string> rplKickArgs = Utils::anyToVec(requestor.getNickname(), requestor.getUsername(),
        requestor.getHostname(), userToKick, channel.getName());
    if (args.size() == 3)
    {
        const std::string& comment = args[2];
        rplKickArgs.push_back(comment);
    }
    Replier::broadcast(clientsFdList, Replier::rplKick, rplKickArgs);
}