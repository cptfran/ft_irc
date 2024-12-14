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

// TODO: verify why irssi sends PART command after joining a channel.
void Join::execute(Server& server, Client& client, const std::vector<std::string>& args) const
{
	const std::string& serverName = server.getName();
	const int clientFd = client.getFd();

	if (args.empty())
	{
		const std::string command = "JOIN";
		Replier::reply(clientFd, Replier::errNeedMoreParams, Utils::anyToVec(serverName, command));
		return;
	}

	if (!client.registered(server.getPassword()))
	{
		Replier::reply(clientFd, Replier::errNotRegistered, Utils::anyToVec(serverName));
		return;
	}

	const std::string& channelName = args[0];

	// Check if the client joining didn't surpass maximum number of joined channels.
	// If yes, don't join and send proper reply.
	const int numOfChannelsClientJoined = client.getNumChannelsJoined();
	if (numOfChannelsClientJoined == CHANNELS_MAX)
	{
		Replier::reply(clientFd, Replier::errTooManyChannels, Utils::anyToVec(server.getName(), channelName));
		return;
	}

	Channel* channelToJoin;

	// Check available channels list to find required channel.
	// If channel is not found, create new channel and add it to the server's channels list.
	try
	{
		channelToJoin = &server.findChannel(channelName);
	}
	catch (const std::exception&)
	{
		server.addChannel(Channel(channelName));
		channelToJoin = &server.findChannel(channelName);
	}

	// If channel is invite only and client is not invited, don't join the client to it and send proper reply.
	const std::string& clientNickname = client.getNickname();
	if (channelToJoin->isInviteOnly() && !channelToJoin->isUserInvited(clientNickname))
	{
		Replier::reply(clientFd, Replier::errInviteOnlyChan, Utils::anyToVec(server.getName(), channelName));
		return;
	}

	// Check if channel requires key, if yes check if it's provided and correct.
	const std::string& channelKey = channelToJoin->getKey();
	if (!channelKey.empty())
	{
		if (args[1].empty())
		{
			Replier::reply(clientFd, Replier::errBadChannelKey, Utils::anyToVec(serverName, channelName));
			return;
		}
		const std::string& clientKey = args[1];
		if (channelKey != clientKey)
		{
			Replier::reply(clientFd, Replier::errBadChannelKey, Utils::anyToVec(serverName, channelName));
			return;
		}
	}

	// Join client to the channel.
	channelToJoin->joinClient(client);
	client.setNumChannelsJoined(numOfChannelsClientJoined + 1);

	// Check if the channel has a topic set, based on the check send proper reply.
	const std::string& channelTopic = channelToJoin->getTopic();
	if (channelTopic.empty())
	{
		Replier::reply(clientFd, Replier::rplNoTopic, Utils::anyToVec(serverName, channelName));
	}
	else
	{
		Replier::reply(clientFd, Replier::rplTopic, Utils::anyToVec(serverName, channelName, channelTopic));
	}

	// Prepare list of arguments to pass to rplNamReply function, which requires server name, channel name and list of
	// nicknames with operator info.
	std::vector<std::string> rplNamReplyArgs = Utils::anyToVec(serverName, channelName);
	std::vector<std::string> channelsNicknamesList = channelToJoin->getNicknamesListWithOperatorInfo();
	for (std::vector<std::string>::iterator it = channelsNicknamesList.begin(); it != channelsNicknamesList.end(); ++it)
	{
		rplNamReplyArgs.push_back(*it);
	}

	// Send replies after client successfuly joined the channel.
	Replier::reply(clientFd, Replier::rplNamReply, rplNamReplyArgs);
	Replier::reply(clientFd, Replier::rplEndOfNames, Utils::anyToVec(serverName, channelName));
}
