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
    Channel* channel;

    // Fetch channel from server's channel list.
    try
    {
        channel = &server.findChannel(channelName);
    }
    // Channel not found.
    catch (const std::exception&)
    {
        Replier::reply(clientFd, Replier::errNotOnChannel, Utils::anyToVec(serverName, channelName));
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

    std::string topic;

    // Only channel name provided by client. Sending back the topic if the topic is set.
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

    // Check if topic change is restricted and only operators can change it.
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