#include "commands/Topic.h"
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
    const std::string& serverName = server.getName();
    const int clientFd = client.getFd();

    // Not enough parameters provided.
    if (args.empty())
    {
        const std::string command = "TOPIC";
        Replier::reply(clientFd, Replier::errNeedMoreParams, Utils::anyToVec(serverName, command));
        return;
    }

    // Client is not registered.
    if (!client.registered(server.getPassword()))
    {
        Replier::reply(clientFd, Replier::errNotRegistered, Utils::anyToVec(serverName));
        return;
    }

    const std::string& channelName = args[0];

    // Fetch channel from server's channel list.
    Channel* channel = server.findChannel(channelName);

    // Channel not found.
    if (channel == NULL)
    {
        Replier::reply(clientFd, Replier::errNotOnChannel, Utils::anyToVec(serverName, channelName));
        return;
    }

    // Fetch client data from channel's client list.
    const Channel::ClientData* clientData = channel->findClientData(client);

    // Client not on the channel.
    if (clientData == NULL)
    {
        Replier::reply(clientFd, Replier::errNotOnChannel, Utils::anyToVec(serverName, channelName));
        return;
    }

    std::string topic;
    if (args.size() == 1)
    {
        topic = channel->getTopic();
        if (topic.empty())
        {
            Replier::reply(clientFd, Replier::rplNoTopic, Utils::anyToVec(serverName, channelName));
            return;
        }
        Replier::reply(clientFd, Replier::rplTopic, Utils::anyToVec(serverName, channelName, topic));
        return;
    }

    // Check if topic is restricted and only operators can use it.
    if (channel->isTopicRestricted() && !clientData->isOperator)
    {
        Replier::reply(clientFd, Replier::errChanOPrivsNeeded, Utils::anyToVec(serverName, channelName));
        return;
    }

    // Set the new topic.
    topic = args[1].substr(1);
    channel->setTopic(topic);

    // Broadcast new topic to all channel members.
    const std::vector<int> clientsFdList = channel->getFdsList();
    Replier::broadcast(clientsFdList, Replier::rplTopic, Utils::anyToVec(serverName, channelName, topic));
}