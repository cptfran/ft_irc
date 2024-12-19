#include "commands/Topic.h"

#include <sys/socket.h>

#include "Log.h"
#include "Server.h"
#include "Utils.h"
#include "Replier.h"

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
            std::string("TOPIC")));
        return;
    }

    // Client is not registered.
    if (!client.registered(server.getPassword()))
    {
        Replier::reply(client.getFd(), Replier::errNotRegistered, Utils::anyToVec(server.getName()));
        return;
    }

    // Fetch channel from server's channel list.
    const std::string& channelName = args[0];
    Channel* channel = server.getChannel(channelName);

    // Channel not found.
    if (channel == NULL)
    {

        Replier::reply(client.getFd(), Replier::errNotOnChannel, Utils::anyToVec(server.getName(), channelName));
        return;
    }

    // Client not on the channel.
    const std::string& clientNickname = client.getNickname();
    if (!channel->isUserOnChannel(clientNickname))
    {
        Replier::reply(client.getFd(), Replier::errNotOnChannel, Utils::anyToVec(server.getName(), channelName));
        return;
    }

    // Only channel name provided by client. Sending back the topic if the topic is set.
    if (args.size() == 1)
    {
        sendTopic(*channel, client.getFd(), server.getName());
        return;
    }

    // Check if topic change is restricted and only operators can change it.
    if (channel->isTopicRestricted() && !channel->isUserOperator(clientNickname))
    {
        Replier::reply(client.getFd(), Replier::errChanOPrivsNeeded, Utils::anyToVec(server.getName(), channelName));
        return;
    }

    setTopic(args, *channel, server.getName());
}

void Topic::sendTopic(const Channel& channel, const int requestorFd, const std::string& serverName) const
{
   const std::string& topic = channel.getTopic();

    DEBUG_LOG("SENDING TOPIC");
    if (topic.empty())
    {
        Replier::reply(requestorFd, Replier::rplNoTopic, Utils::anyToVec(serverName, channel.getName()));
        return;
    }

    Replier::reply(requestorFd, Replier::rplTopic, Utils::anyToVec(serverName, channel.getName(), topic));
}

void Topic::setTopic(const std::vector<std::string>& args, Channel& channel, const std::string& serverName) const
{
    // Set the new topic.
    const std::string& topic = args[1].substr(1);
    channel.setTopic(topic);

    // Broadcast new topic to all channel members.
    const std::vector<int> clientsFdList = channel.getFdsList();
    Replier::broadcast(clientsFdList, Replier::rplTopic, Utils::anyToVec(serverName, channel.getName(), topic));
}