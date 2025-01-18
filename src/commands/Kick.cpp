#include "commands/Kick.h"

#include "data/Channel.h"
#include "core/Server.h"
#include "utils/Utils.h"
#include "communication/Replier.h"

Kick::Kick() : Command()
{

}

Kick::~Kick()
{

}

void Kick::execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const
{
    ConfigManager& configManager = serverManager.getConfigManager();

    // Not enough parameters provided.
    if (args.size() < 2)
    {
        Replier::addToQueue(requester.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(configManager.getName(),
            requester.getNickname(), std::string("KICK")));
        return;
    }

    // Find the channel.
    const std::string& channelName = args[0];
    Channel* channel = serverManager.getChannelManager().getChannel(channelName);

    // Channel not found.
    if (channel == NULL)
    {
        Replier::addToQueue(requester.getFd(), Replier::errNoSuchChannel, Utils::anyToVec(configManager.getName(),
            requester.getNickname(), channelName));
        return;
    }

    // Client not on the channel.
    if (!channel->isUserOnChannel(requester.getNickname()))
    {
        Replier::addToQueue(requester.getFd(), Replier::errNotOnChannel, Utils::anyToVec(configManager.getName(),
            requester.getNickname(), channelName));
        return;
    }

    // Client is not an operator.
    if (!channel->isUserOperator(requester.getNickname()))
    {
        Replier::addToQueue(requester.getFd(), Replier::errChanOPrivsNeeded, Utils::anyToVec(configManager.getName(),
            requester.getNickname(), channelName));
        return;
    }

    kickUser(args, *channel, requester, configManager.getName());
}

void Kick::kickUser(const std::vector<std::string>& args, Channel& channel, const Client& requester,
    const std::string& serverName) const
{
    const std::string& userToKick = args[1];

    // Get client list for broadcast message before client is kicked, so it will receive kick information.
    const std::vector<Client>& clientList = channel.getClientList();

    // Kick user from the channel.
    if (!channel.deleteUser(userToKick))
    {
        Replier::addToQueue(requester.getFd(), Replier::errUserNotInChannel,
            Utils::anyToVec(serverName, requester.getNickname(), userToKick, channel.getName()));
        return;
    }

    // Broadcast reply after kick.
    std::vector<std::string> rplKickArgs = Utils::anyToVec(requester.getNickname(), requester.getUsername(),
        requester.getHostname(), userToKick, channel.getName());
    if (args.size() == 3)
    {
        const std::string& comment = args[2];
        rplKickArgs.push_back(comment);
    }

    for (std::vector<Client>::const_iterator it = clientList.begin(); it != clientList.end(); ++it)
    {
        Replier::addToQueue(it->getFd(), Replier::rplKick, rplKickArgs);
    }
}