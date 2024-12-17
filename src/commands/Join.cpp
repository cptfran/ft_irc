#include "commands/Join.h"
#include "Log.h"
#include "Server.h"
#include "Utils.h"
#include "Replier.h"

Join::Join()
{

}

Join::~Join()
{

}

void Join::execute(Server& server, Client& client, const std::vector<std::string>& args) const
{
	// Not enough arguments provided.
	if (args.empty())
	{
		handleMissingParams("JOIN", client.getFd(), server.getName());
		return;
	}

	// User is not registered.
	if (!client.registered(server.getPassword()))
	{
		handleNotRegistered(client.getFd(), server.getName());
		return;
	}

	const std::string& channelName = args[0];

	// Check if the client joining didn't surpass maximum number of joined channels.
	// If yes, don't join and send proper reply.
	if (client.getNumChannelsJoined() == CHANNELS_MAX)
	{
		handleTooManyChannels(client.getFd(), server.getName(), channelName);
		return;
	}

	// Find channel on the list. If not found, create new channel.
	Channel* channelToJoin = findOrCreateChannel(server, channelName);

	// If channel is invite only and client is not invited, don't join the client to it and send proper reply.
	if (channelToJoin->isInviteOnly() && !channelToJoin->isUserInvited(client.getNickname()))
	{
		handleInviteOnly(client.getFd(), server.getName(), channelName);
		return;
	}

	// Check if channel requires key, if yes, check if it's provided and correct.
	if (!channelToJoin->getKey().empty() && !isValidChannelKey(args, channelToJoin->getKey()))
	{
		handleInvalidChannelKey(client.getFd(), server.getName(), channelName);
		return;
	}

	joinChannel(client, channelToJoin, server.getName());
}

void Join::handleNotRegistered(const int clientFd, const std::string& serverName) const
{
	Replier::reply(clientFd, Replier::errNotRegistered, Utils::anyToVec(serverName));
}

void Join::handleTooManyChannels(const int clientFd, const std::string& serverName,
	const std::string& channelName) const
{
	Replier::reply(clientFd, Replier::errTooManyChannels, Utils::anyToVec(serverName, channelName));
}

Channel* Join::findOrCreateChannel(Server& server, const std::string& channelName) const
{
	Channel* channelToJoin;

	try
	{
		channelToJoin = &server.getChannel(channelName);
	}
	catch (const std::exception&)
	{
		server.addChannel(Channel(channelName));
		channelToJoin = &server.getChannel(channelName);
	}

	return channelToJoin;
}

void Join::handleInviteOnly(const int clientFd, const std::string& serverName, const std::string& channelName) const
{
	Replier::reply(clientFd, Replier::errInviteOnlyChan, Utils::anyToVec(serverName, channelName));
}

bool Join::isValidChannelKey(const std::vector<std::string>& args, const std::string& channelKey) const
{
	if (args[1].empty())
	{
		return false;
	}

	const std::string& clientKey = args[1];

	if (channelKey != clientKey)
	{
		return false;
	}

	return true;
}

void Join::handleInvalidChannelKey(const int clientFd, const std::string& serverName,
	const std::string& channelName) const
{
	Replier::reply(clientFd, Replier::errBadChannelKey, Utils::anyToVec(serverName, channelName));
}

void Join::joinChannel(Client& client, Channel* channelToJoin, const std::string& serverName) const
{
	channelToJoin->joinUser(client);
	client.setNumChannelsJoined(client.getNumChannelsJoined() + 1);

	std::cout << channelToJoin->getName() << std::endl;

	const std::string& channelTopic = channelToJoin->getTopic();
	if (channelTopic.empty())
	{
		Replier::reply(client.getFd(), Replier::rplNoTopic, Utils::anyToVec(serverName, channelToJoin->getName()));
	}
	else
	{
		Replier::reply(client.getFd(), Replier::rplTopic, Utils::anyToVec(serverName, channelToJoin->getName(),
			channelTopic));
	}

	std::vector<std::string> rplNamReplyArgs = Utils::anyToVec(serverName, channelToJoin->getName());
	std::vector<std::string> channelsNicknamesList = channelToJoin->getNicknamesListWithOperatorInfo();
	rplNamReplyArgs.insert(rplNamReplyArgs.end(), channelsNicknamesList.begin(),
		channelsNicknamesList.end());

	Replier::reply(client.getFd(), Replier::rplNamReply, rplNamReplyArgs);
	Replier::reply(client.getFd(), Replier::rplEndOfNames, Utils::anyToVec(serverName, channelToJoin->getName()));
}