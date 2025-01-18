#include "commands/Topic.h"
#include <sys/socket.h>
#include "core/Server.h"
#include "utils/Utils.h"
#include "communication/Replier.h"
#include "data/Channel.h"
#include "communication/ClientTranslator.h"

Topic::Topic() : Command()
{

}

Topic::~Topic()
{

}

void Topic::execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const
{
    ConfigManager& configManager = serverManager.getConfigManager();

    // Not enough parameters provided.
    if (args.empty())
    {
        Replier::addToQueue(requester.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(configManager.getName(),
            requester.getNickname(), std::string("TOPIC")));
        return;
    }

    // Client is not registered.
    if (!requester.registered(configManager.getPassword()))
    {
        Replier::addToQueue(requester.getFd(), Replier::errNotRegistered, Utils::anyToVec(configManager.getName(),
            requester.getNickname()));
        return;
    }

    // Fetch channel from server's channel list.
    const std::string& channelName = args[0];
    Channel* channel = serverManager.getChannelManager().getChannel(channelName);

    // Channel not found.
    if (channel == NULL)
    {

        Replier::addToQueue(requester.getFd(), Replier::errNotOnChannel, Utils::anyToVec(configManager.getName(),
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

    // Only channel name provided by client. Sending back the topic if the topic is set.
    if (args.size() == 1)
    {
        sendTopic(*channel, requester, configManager.getName());
        return;
    }

    // Check if topic change is restricted and only operators can change it.
    if (channel->isTopicRestricted() && !channel->isUserOperator(requester.getNickname()))
    {
        Replier::addToQueue(requester.getFd(), Replier::errChanOPrivsNeeded, Utils::anyToVec(configManager.getName(),
            requester.getNickname(), channelName));
        return;
    }

    setTopic(args, requester, *channel, configManager.getName());
}

void Topic::setTopic(const std::vector<std::string>& args, const Client& requester, Channel& channel,
    const std::string& serverName) const
{
    // Set the new topic.
    const std::string& topic = args[1];
    if (topic[0] != ':')
    {
        Replier::addToQueue(requester.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(serverName,
            requester.getNickname(), std::string("TOPIC")));
        return;
    }
    channel.setTopic(ClientTranslator::sanitizeColonMessage(topic));

    // Broadcast new topic to all channel members.

    const std::vector<Client> clientsList = channel.getClientList();
    for (std::vector<Client>::const_iterator it = clientsList.begin(); it != clientsList.end(); ++it)
    {
        Replier::addToQueue(it->getFd(), Replier::rplTopic, Utils::anyToVec(serverName, it->getNickname(),
            channel.getName(), channel.getTopic()));
    }
}
