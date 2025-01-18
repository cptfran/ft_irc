#include "commands/Command.h"
#include "communication/Replier.h"
#include "data/Channel.h"
#include "utils/Utils.h"
#include "manager/Manager.h"

Command::Command()
{

}

Command::~Command()
{

}

/**
 * @brief Sends the topic of the specified channel to the requester.
 *
 * This method checks if the channel has a topic set. If the topic is empty,
 * it sends a "no topic" reply to the requester. Otherwise, it sends the
 * current topic of the channel to the requester.
 *
 * @param channel The channel whose topic is to be sent.
 * @param requester The client requesting the topic.
 * @param serverName The name of the server sending the reply.
 */
void Command::sendTopic(const Channel& channel, const Client& requester, const std::string& serverName) const
{
	const std::string& topic = channel.getTopic();

	if (topic.empty())
	{
		Replier::addToQueue(requester.getFd(), Replier::rplNoTopic, Utils::anyToVec(serverName, requester.getNickname(),
			channel.getName()));
		return;
	}

	Replier::addToQueue(requester.getFd(), Replier::rplTopic, Utils::anyToVec(serverName, requester.getNickname(),
		channel.getName(), topic));
}