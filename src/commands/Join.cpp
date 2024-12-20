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
		Replier::reply(client.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(server.getName(),
			std::string("JOIN")));
		return;
	}

	// User is not registered.
	if (!client.registered(server.getPassword()))
	{
		Replier::reply(client.getFd(), Replier::errNotRegistered, Utils::anyToVec(server.getName()));
		return;
	}

	const std::string& channelName = args[0];

	// Check if the client joining didn't surpass maximum number of joined channels.
	// If yes, don't join and send proper reply.
	if (client.getNumChannelsJoined() == CHANNELS_MAX)
	{
		Replier::reply(client.getFd(), Replier::errTooManyChannels, Utils::anyToVec(server.getName(), channelName));
		return;
	}

	// Find channel on the list. If not found, create new channel.
	Channel* channelToJoin = findOrCreateChannel(server, channelName);

	// If channel is invite only and client is not invited, don't join the client to it and send proper reply.
	if (channelToJoin->isInviteOnly() && !channelToJoin->isUserInvited(client.getNickname()))
	{
		Replier::reply(client.getFd(), Replier::errInviteOnlyChan, Utils::anyToVec(server.getName(), channelName));
		return;
	}

	// Check if channel requires key, if yes, check if it's provided and correct.
	if (!channelToJoin->getKey().empty() && !isValidChannelKey(args, channelToJoin->getKey()))
	{
		Replier::reply(client.getFd(), Replier::errBadChannelKey, Utils::anyToVec(server.getName(), channelName));
		return;
	}

	// Check if channel has user limit, if yes check if it's not full.
	if (channelToJoin->isUserLimitActive() && channelToJoin->getUserLimit() == channelToJoin->getNumOfJoinedUsers())
	{
		Replier::reply(client.getFd(), Replier::errChannelIsFull, Utils::anyToVec(server.getName(), channelName));
		return;
	}

	joinChannel(client, *channelToJoin, server.getName());
}

Channel* Join::findOrCreateChannel(Server& server, const std::string& channelName) const
{
	Channel* channelToJoin = server.getChannel(channelName);
	if (channelToJoin == NULL)
	{
		server.addChannel(Channel(channelName));
		channelToJoin = server.getChannel(channelName);
	}

	return channelToJoin;
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

void Join::joinChannel(Client& client, Channel& channelToJoin, const std::string& serverName) const
{
	channelToJoin.joinUser(client);
	client.setNumChannelsJoined(client.getNumChannelsJoined() + 1);

	Replier::reply(client.getFd(), Replier::rplJoin, Utils::anyToVec(client.getNickname(), client.getUsername(),
		client.getHostname(), channelToJoin.getName()));

	sendTopic(channelToJoin, client, serverName);

	std::vector<std::string> rplNamReplyArgs = Utils::anyToVec(serverName, client.getNickname(),
		channelToJoin.getName());
	std::vector<std::string> channelsNicknamesList = channelToJoin.getNicknamesListWithOperatorInfo();
	rplNamReplyArgs.insert(rplNamReplyArgs.end(), channelsNicknamesList.begin(),
		channelsNicknamesList.end());

	Replier::reply(client.getFd(), Replier::rplNamReply, rplNamReplyArgs);
	Replier::reply(client.getFd(), Replier::rplEndOfNames, Utils::anyToVec(serverName, channelToJoin.getName()));
}