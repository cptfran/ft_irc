#include "commands/Command.h"
#include "Replier.h"
#include "Server.h"
#include "Utils.h"

Command::Command()
{

}

Command::~Command()
{

}

void Command::handleMissingParams(const std::string& command, const int clientFd, const std::string& serverName) const
{
	Replier::reply(clientFd, Replier::errNeedMoreParams, Utils::anyToVec(serverName, command));
}

Channel* Command::findChannel(Server& server, const std::string& channelToFindName) const
{
	try
	{
		return &server.getChannel(channelToFindName);
	}
	catch (const std::exception&)
	{
		return NULL;
	}
}

void Command::handleChannelNotFound(const Client& requestingClient, const std::string& serverName,
	const Client& invitedClient, const std::string& channelName) const
{
	(void)invitedClient;
	Replier::reply(requestingClient.getFd(), Replier::errNoSuchChannel, Utils::anyToVec(serverName, channelName));
}

void Command::handleUserNotOnChannel(const int requestingFd, const std::string& serverName,
	const std::string& channelName) const
{
	Replier::reply(requestingFd, Replier::errNotOnChannel, Utils::anyToVec(serverName, channelName));
}

void Command::handleNoOperatorPriv(const int requestingFd, const std::string& serverName,
	const std::string& channelName) const
{
	Replier::reply(requestingFd, Replier::errChanOPrivsNeeded,  Utils::anyToVec(serverName, channelName));
}

void Command::handleUserNotInChannel(const int requestingFd, const std::string& serverName, const std::string& nickname,
	const std::string& channelName) const
{
	Replier::reply(requestingFd, Replier::errUserNotInChannel,  Utils::anyToVec(serverName, nickname, channelName));
}
