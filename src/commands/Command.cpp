#include "commands/Command.h"
#include "channel/Channel.h"
#include "replier/Replier.h"
#include "utils/Utils.h"

Command::Command()
{

}

Command::~Command()
{

}

void Command::sendTopic(const Channel& channel, const Client& requester, const std::string& serverName) const
{
	const std::string& topic = channel.getTopic();

	if (topic.empty())
	{
		Replier::reply(requester.getFd(), Replier::rplNoTopic, Utils::anyToVec(serverName, requester.getNickname(),
			channel.getName()));
		return;
	}

	Replier::reply(requester.getFd(), Replier::rplTopic, Utils::anyToVec(serverName, requester.getNickname(),
		channel.getName(), topic));
}