#include "commands/Join.h"
#include "core/Log.h"
#include "core/Server.h"
#include "utils/Utils.h"
#include "communication/Replier.h"

Join::Join() : Command()
{

}

Join::~Join()
{

}

void Join::execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const
{
	ConfigManager& configManager = serverManager.getConfigManager();

	// Not enough arguments provided.
	if (args.empty())
	{
		Replier::addToQueue(requester.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(configManager.getName(),
			requester.getNickname(), std::string("JOIN")));
		return;
	}

	// User is not registered.
	if (!requester.registered(configManager.getPassword()))
	{
		Replier::addToQueue(requester.getFd(), Replier::errNotRegistered, Utils::anyToVec(configManager.getName(),
			requester.getNickname()));
		return;
	}

	const std::string& channelName = args[0];

	// Check if the client joining didn't surpass maximum number of joined channels.
	// If yes, don't join and send proper reply.
	if (requester.getNumChannelsJoined() == CHANNELS_MAX)
	{
		Replier::addToQueue(requester.getFd(), Replier::errTooManyChannels, Utils::anyToVec(configManager.getName(),
			requester.getNickname(), channelName));
		return;
	}

	// Find channel on the list. If not found, create new channel.
	Channel* channelToJoin = findOrCreateChannel(serverManager.getChannelManager(), channelName);

	// If channel is invite only and client is not invited, don't join the client to it and send proper reply.
	if (channelToJoin->isInviteOnly() && !channelToJoin->isUserInvited(requester.getNickname()))
	{
		Replier::addToQueue(requester.getFd(), Replier::errInviteOnlyChan, Utils::anyToVec(configManager.getName(),
			requester.getNickname(), channelName));
		return;
	}

	// Check if channel requires key, if yes, check if it's provided and correct.
	if (!channelToJoin->getKey().empty() && !isValidChannelKey(args, channelToJoin->getKey()))
	{
		Replier::addToQueue(requester.getFd(), Replier::errBadChannelKey, Utils::anyToVec(configManager.getName(),
			requester.getNickname(), channelName));
		return;
	}

	// Check if channel has user limit, if yes check if it's not full.
	if (channelToJoin->isUserLimitActive() && channelToJoin->getUserLimit() == channelToJoin->getNumOfJoinedUsers())
	{
		Replier::addToQueue(requester.getFd(), Replier::errChannelIsFull, Utils::anyToVec(configManager.getName(),
			requester.getNickname(), channelName));
		return;
	}

	joinChannel(requester, *channelToJoin, configManager.getName());
}

Channel* Join::findOrCreateChannel(ChannelManager& channelManager, const std::string& channelName) const
{
	Channel* channelToJoin = channelManager.getChannel(channelName);
	if (channelToJoin == NULL)
	{
		channelManager.addChannel(Channel(channelName));
		channelToJoin = channelManager.getNewestChannel();
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

	Replier::addToQueue(client.getFd(), Replier::rplJoin, Utils::anyToVec(client.getNickname(), client.getUsername(),
		client.getHostname(), channelToJoin.getName()));

	sendTopic(channelToJoin, client, serverName);

	std::vector<std::string> rplNamReplyArgs = Utils::anyToVec(serverName, client.getNickname(),
		channelToJoin.getName());
	std::vector<std::string> channelsNicknamesList = channelToJoin.getNicknamesListWithOperatorInfo();
	rplNamReplyArgs.insert(rplNamReplyArgs.end(), channelsNicknamesList.begin(), channelsNicknamesList.end());

	Replier::addToQueue(client.getFd(), Replier::rplNamReply, rplNamReplyArgs);
	Replier::addToQueue(client.getFd(), Replier::rplEndOfNames, Utils::anyToVec(serverName, client.getNickname(),
		channelToJoin.getName()));
}