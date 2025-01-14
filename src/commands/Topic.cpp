#include "commands/Topic.h"
#include <sys/socket.h>
#include "server/Server.h"
#include "utils/Utils.h"
#include "replier/Replier.h"
#include "channel/Channel.h"
#include "client/ClientTranslator.h"

Topic::Topic()
{

}

Topic::~Topic()
{

}

void Topic::execute(Server& server, Client& requester, const std::vector<std::string>& args) const
{
    // Not enough parameters provided.
    if (args.empty())
    {
        Replier::reply(requester.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(server.getName(),
            requester.getNickname(), std::string("TOPIC")));
        return;
    }

    // Client is not registered.
    if (!requester.registered(server.getPassword()))
    {
        Replier::reply(requester.getFd(), Replier::errNotRegistered, Utils::anyToVec(server.getName(),
            requester.getNickname()));
        return;
    }

    // Fetch channel from server's channel list.
    const std::string& channelName = args[0];
    Channel* channel = server.getChannel(channelName);

    // Channel not found.
    if (channel == NULL)
    {

        Replier::reply(requester.getFd(), Replier::errNotOnChannel, Utils::anyToVec(server.getName(),
            requester.getNickname(), channelName));
        return;
    }

    // Client not on the channel.
    if (!channel->isUserOnChannel(requester.getNickname()))
    {
        Replier::reply(requester.getFd(), Replier::errNotOnChannel, Utils::anyToVec(server.getName(),
            requester.getNickname(), channelName));
        return;
    }

    // Only channel name provided by client. Sending back the topic if the topic is set.
    if (args.size() == 1)
    {
        sendTopic(*channel, requester, server.getName());
        return;
    }

    // Check if topic change is restricted and only operators can change it.
    if (channel->isTopicRestricted() && !channel->isUserOperator(requester.getNickname()))
    {
        Replier::reply(requester.getFd(), Replier::errChanOPrivsNeeded, Utils::anyToVec(server.getName(),
            requester.getNickname(), channelName));
        return;
    }

    setTopic(args, requester, *channel, server.getName());
}

void Topic::setTopic(const std::vector<std::string>& args, const Client& requester, Channel& channel,
    const std::string& serverName) const
{
    // Set the new topic.
    const std::string& topic = args[1];
    if (topic[0] != ':')
    {
        Replier::reply(requester.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(serverName,
            requester.getNickname(), std::string("TOPIC")));
        return;
    }
    channel.setTopic(ClientTranslator::sanitizeColonMessage(topic));

    // Broadcast new topic to all channel members.
    const std::vector<int> clientsFdList = channel.getFdsList();
    Replier::broadcast(clientsFdList, Replier::rplTopic, Utils::anyToVec(serverName, requester.getNickname(),
        channel.getName(), channel.getTopic()));
}
