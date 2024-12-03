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
	const std::string& serverName = server.getName();

	if (!client.registered(server.getPassword()))
	{
		Replier::reply(client, Replier::errNotRegistered, Utils::anyToVec(serverName));
		return;
	}

	const std::string& channelName = args[0];

	// Check if the client joining didn't surpass maximum number of joined channels.
	// If yes, don't join and send proper reply.
	const int numOfChannelsClientJoined = client.getNumChannelsJoined();
	if (numOfChannelsClientJoined == CHANNELS_MAX)
	{
		Replier::reply(client, Replier::errTooManyChannels, Utils::anyToVec(server.getName(), channelName));
		return;
	}

	// Check available channels list to find required channel.
	Channel* channelToJoin = server.findChannel(channelName);

	// If channel is not found, create new channel and add it to the server's channels list.
	if (channelToJoin == NULL)
	{
		server.addChannel(Channel(channelName, client, true));
		channelToJoin = server.findChannel(channelName);
		if (channelToJoin == NULL)
		{
			throw std::runtime_error(ERROR FIND_CHANNEL_FAIL_AFTER_ADD);
		}
	}

	// If channel is invite only, don't join the client to it and send proper reply.
	if (channelToJoin->isInviteOnly())
	{
		Replier::reply(client, Replier::errInviteOnlyChan, Utils::anyToVec(server.getName(), channelName));
		return;
	}

	// Join client to the channel.
	channelToJoin->joinClient(client);
	client.setNumChannelsJoined(numOfChannelsClientJoined + 1);

	// Check if the channel has a topic set, based on the check send proper reply.
	const std::string& channelTopic = channelToJoin->getTopic();
	if (channelTopic.empty())
	{
		Replier::reply(client, Replier::rplNoTopic, Utils::anyToVec(serverName, channelName));
	}
	else
	{
		Replier::reply(client, Replier::rplTopic, Utils::anyToVec(serverName, channelName, channelTopic));
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
	Replier::reply(client, Replier::rplNamReply, rplNamReplyArgs);
	Replier::reply(client, Replier::rplEndOfNames, Utils::anyToVec(serverName, channelName));
}
