#include "commands/Topic.h"
#include <sys/socket.h>
#include "Server.h"
#include "Utils.h"
#include "Replier.h"
#include "Channel.h"

Topic::Topic()
{

}

Topic::~Topic()
{

}

void Topic::execute(Server& server, Client& client, const std::vector<std::string>& args) const
{
    // Not enough parameters provided.
    if (args.empty())
    {
        Replier::reply(client.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(server.getName(),
            client.getNickname(), std::string("TOPIC")));
        return;
    }

    // Client is not registered.
    if (!client.registered(server.getPassword()))
    {
        Replier::reply(client.getFd(), Replier::errNotRegistered, Utils::anyToVec(server.getName(),
            client.getNickname()));
        return;
    }

    // Fetch channel from server's channel list.
    const std::string& channelName = args[0];
    Channel* channel = server.getChannel(channelName);

    // Channel not found.
    if (channel == NULL)
    {

        Replier::reply(client.getFd(), Replier::errNotOnChannel, Utils::anyToVec(server.getName(),
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

    // Only channel name provided by client. Sending back the topic if the topic is set.
    if (args.size() == 1)
    {
        sendTopic(*channel, client, server.getName());
        return;
    }

    // Check if topic change is restricted and only operators can change it.
    if (channel->isTopicRestricted() && !channel->isUserOperator(client.getNickname()))
    {
        Replier::reply(client.getFd(), Replier::errChanOPrivsNeeded, Utils::anyToVec(server.getName(),
            client.getNickname(), channelName));
        return;
    }

    setTopic(args, client.getNickname(), *channel, server.getName());
}

void Topic::setTopic(const std::vector<std::string>& args, const std::string& requestorNickname, Channel& channel,
    const std::string& serverName) const
{
    // Set the new topic.
    const std::string& topic = args[1];
    channel.setTopic(Utils::sanitizeColonMessage(topic));

    // Broadcast new topic to all channel members.
    const std::vector<int> clientsFdList = channel.getFdsList();
    Replier::broadcast(clientsFdList, Replier::rplTopic, Utils::anyToVec(serverName, requestorNickname,
        channel.getName(), channel.getTopic()));
}
