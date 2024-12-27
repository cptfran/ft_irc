#include "commands/Command.h"

#include "Log.h"
#include "Replier.h"
#include "Server.h"
#include "Utils.h"

Command::Command()
{

}

Command::~Command()
{

}

void Command::sendTopic(const Channel& channel, const Client& requestor, const std::string& serverName) const
{
	const std::string& topic = channel.getTopic();

	if (topic.empty())
	{
		Replier::reply(requestor.getFd(), Replier::rplNoTopic, Utils::anyToVec(serverName, requestor.getNickname(),
			channel.getName()));
		return;
	}

	Replier::reply(requestor.getFd(), Replier::rplTopic, Utils::anyToVec(serverName, requestor.getNickname(),
		channel.getName(), topic));
}