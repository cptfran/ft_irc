#include "../include/Replier.h"
#include <sys/socket.h>
#include "Log.h"

Replier::Replier()
{

}

Replier::~Replier()
{

}

// TODO: probably the best pattern for message is :<servername> <number> <nickname> <rest>, check all the messages
// (last fixed, rplTopic).
void Replier::reply(const int clientFd, const ReplyFunction func, const std::vector<std::string>& args)
{
	const std::string reply = func(args);
	send(clientFd, reply.c_str(), reply.size(), 0);
}

void Replier::broadcast(const std::vector<int>& clientsFdList, const ReplyFunction func,
	const std::vector<std::string>& args)
{
	for (std::vector<int>::const_iterator it = clientsFdList.begin(); it != clientsFdList.end(); ++it)
	{
		Replier::reply(*it, func, args);
	}
}

std::string Replier::rplWelcome(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplWelcome()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& username = args[2];

	return ":" + serverName + " 001 " + nickname + " :Welcome to the Internet Relay Network " + nickname + "!"
	+ username + "@" + serverName + "\r\n";
}

std::string Replier::rplYourHost(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplYourHost()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& version = args[2];

	return ":" + serverName + " 002 " + nickname + " :Your host is " + serverName + ", running version " + version
	+ "\r\n";
}

std::string Replier::rplCreated(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplCreated()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& creationDate = args[2];

	return ":" + serverName + " 003 " + nickname + ": This server was created " + creationDate + "\r\n";
}

std::string Replier::rplMyInfo(const std::vector<std::string>& args)
{
	if (args.size() != 4)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplMyInfo()"));
	}

	const std::string& serverName = args[0];
	const std::string& version = args[1];
	const std::string& availableUserModes = args[2];
	const std::string& availableChannelModes = args[3];

	return ":" + serverName + " 004 " + serverName + " Version: " + version + " " + availableUserModes
	+ " Available channel modes: " + availableChannelModes + "\r\n";
}

std::string Replier::rplPong(const std::vector<std::string>& args)
{
	if (args.empty())
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplPong()"));
	}

	const std::string& serverName = args[0];

	if (args.size() == 1)
	{
		return "PONG :" + serverName + "\r\n";
	}

	const std::string& serverToReply = args[1];

	return "PONG :" + serverName + " " + serverToReply + "\r\n";
}

std::string Replier::rplCap(const std::vector<std::string>& args)
{
	if (args.empty())
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplCap()"));
	}

	const std::string& serverName = args[0];

	return ":" + serverName + ": CAP * LS :\r\n";
}

std::string Replier::rplKick(const std::vector<std::string>& args)
{
	if (args.size() < 5)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplKick()"));
	}

	const std::string& kickerNickname = args[0];
	const std::string& kickerUsername = args[1];
	const std::string& kickerHostname = args[2];
	const std::string& kickedUser = args[3];
	const std::string& channelName = args[4];

	if (args.size() == 6)
	{
		const std::string& comment = args[5].substr(0);
		return ":" + kickerNickname + "!" + kickerUsername + "@" + kickerHostname + " KICK " + channelName + " " +
			kickedUser + " :" + comment + "\r\n";
	}
	return ":" + kickerNickname + "!" + kickerUsername + "@" + kickerHostname + " KICK " + channelName + " " +
		kickedUser + "\r\n";
}

std::string Replier::rplChannelModeIs(const std::vector<std::string>& args)
{
	if (args.size() < 2)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplChannelModeIs()"));
	}

	const std::string& serverName = args[0];
	const std::string& channelName = args[1];
	std::string modeParams;
	for (size_t i = 2; i < args.size(); ++i)
	{
		modeParams += args[i] + " ";
	}

	return ":" + serverName + " 324 " + channelName + " " + modeParams + "\r\n";
}

std::string Replier::rplNoTopic(const std::vector<std::string>& args)
{
	if (args.size() != 2)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplNoTopic()"));
	}

	const std::string& serverName = args[0];
	const std::string& channelName = args[1];

	return ":" + serverName + " 331 " + channelName + " :No topic is set\r\n";
}

std::string Replier::rplTopic(const std::vector<std::string>& args)
{
	if (args.size() != 4)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplTopic()"));
	}

	const std::string& serverName = args[0];
	const std::string& requestorNickname = args[1];
	const std::string& channelName = args[2];
	const std::string& topic = args[3];

	return ":" + serverName + " 332 " + requestorNickname + " " + channelName + " :" + topic + "\r\n";
}

std::string Replier::rplInviting(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplInviting()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& channelName = args[2];

	return "341 " + serverName + " " + nickname + " " + channelName + "\r\n";
}

std::string Replier::rplInvite(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplInvite()"));
	}

	const std::string& invitingNickname = args[0];
	const std::string& invitedNickname = args[1];
	const std::string& channelName = args[2];

	return ":" + invitingNickname + " INVITE " + invitedNickname + " :" + channelName + "\r\n";
}

std::string Replier::rplJoin(const std::vector<std::string>& args)
{
	if (args.size() != 4)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplJoin()"));
	}

	const std::string& nickname = args[0];
	const std::string& username = args[1];
	const std::string& hostname = args[2];
	const std::string& channelName = args[3];

	return ":" + nickname + "!" + username + "@" + hostname + " JOIN :" + channelName + "\r\n";
}

std::string Replier::rplNamReply(const std::vector<std::string>& args)
{
	if (args.size() < 4)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplNamReply()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& channelName = args[2];
	std::vector<std::string> channelNicknames;
	for (size_t i = 3; i < args.size(); ++i)
	{
		channelNicknames.push_back(args[i]);
	}

	std::string reply = ":" + serverName + " 353 " + nickname + " = " + channelName + " :";
	for (size_t i = 0; i < channelNicknames.size(); ++i)
	{
		reply += channelNicknames[i] + " ";
		if (i + 1 == channelNicknames.size())
		{
			reply = reply.substr(0, reply.length() - 1);
		}
	}
	reply += "\r\n";

	DEBUG_LOG(reply);
	return reply;
}

std::string Replier::rplEndOfNames(const std::vector<std::string>& args)
{
	if (args.size() != 2)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplEndOfNames()"));
	}

	const std::string& serverName = args[0];
	const std::string& channelName = args[1];

	return "366 " + serverName + " " + channelName + " :End of /NAMES list\r\n";
}

std::string Replier::errNoSuchNick(const std::vector<std::string>& args)
{
	if (args.size() != 2)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplNoSuchNick()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];

	return "401 " + serverName + " " + nickname + " :No such nick/channel\r\n";
}

std::string Replier::errNoSuchChannel(const std::vector<std::string>& args)
{
	if (args.size() != 2)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errNoSuchChannel()"));
	}

	const std::string& serverName = args[0];
	const std::string& channelName = args[1];

	return "403 " + serverName + " " + channelName + " :No such channel\r\n";
}

std::string Replier::errTooManyChannels(const std::vector<std::string>& args)
{
	if (args.size() != 2)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errTooManyChannels()"));
	}

	const std::string& serverName = args[0];
	const std::string& channelName = args[1];

	return "405 " + serverName + " " + channelName + " :You have joined too many channels\r\n";
}

std::string Replier::errUnknownCommand(const std::vector<std::string>& args)
{
	if (args.size() != 1)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errUnknownCommand()"));
	}

	const std::string& serverName = args[0];

	return "421 " + serverName + " :Unknown command\r\n";
}

std::string Replier::errNickCollision(const std::vector<std::string>& args)
{
	if (args.size() != 1)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errNickCollision()"));
	}

	const std::string& serverName = args[0];

	return "436 " + serverName + " :Nickname collision KILL\r\n";
}

std::string Replier::errUserNotInChannel(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errUserNotInChannel()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& channelName = args[2];

	return "441 " + serverName + " " + nickname + " " + channelName + " :They aren't on that channel\r\n";
}

std::string Replier::errNotOnChannel(const std::vector<std::string>& args)
{
	if (args.size() != 2)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errNotOnChannel()"));
	}

	const std::string& serverName = args[0];
	const std::string& channelName = args[1];

	return "442 " + serverName + " " + channelName + " :You're not on that channel\r\n";
}

std::string Replier::errUserOnChannel(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errUserOnChannel()"));
	}
	const std::string& serverName = args[0];
	const std::string& userToInvite = args[1];
	const std::string& channelName = args[2];

	return "443 " + serverName + " " + userToInvite + " " + channelName + " :is already on channel\r\n";
}

std::string Replier::errNotRegistered(const std::vector<std::string>& args)
{
	if (args.size() != 1)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errNotRegistered()"));
	}

	const std::string& serverName = args[0];

	return "451 " + serverName + " :You have not registered\r\n";
}

std::string Replier::errNeedMoreParams(const std::vector<std::string>& args)
{
	if (args.size() != 2)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errNeedMoreParams()"));
	}

	const std::string& serverName = args[0];
	const std::string& command = args[1];

	return "461 " + serverName + " " + command + " :Not enough parameters\r\n";
}


std::string Replier::errPasswdMismatch(const std::vector<std::string>& args)
{
	if (args.size() != 1)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errPasswdMismatch()"));
	}

	const std::string& serverName = args[0];

	return "464 " + serverName + " :Password incorrect.\r\n";
}

std::string Replier::errKeySet(const std::vector<std::string>& args)
{
	if (args.size() != 2)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errKeySet()"));
	}

	const std::string& serverName = args[0];
	const std::string& channelName = args[1];

	return "467 " + serverName + " " + channelName + " :Channel key already set \r\n";
}

std::string Replier::errChannelIsFull(const std::vector<std::string>& args)
{
	if (args.size() != 2)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errChannelIsFull()"));
	}

	const std::string& serverName = args[0];
	const std::string& channelName = args[1];

	return "471 " + serverName + " " + channelName + " :Cannot join channel (+1)\r\n";
}

std::string Replier::errUnknownMode(const std::vector<std::string>& args)
{
	if (args.size() != 2)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errUnknownMode()"));
	}

	const std::string& serverName = args[0];
	const std::string& modeChar = args[1];

	return "472 " + serverName + " " + modeChar + " :is uknown mode char to me\r\n";
}

std::string Replier::errInviteOnlyChan(const std::vector<std::string>& args)
{
	if (args.size() != 2)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errInviteOnlyChan()"));
	}

	const std::string& serverName = args[0];
	const std::string& channelName = args[1];

	return "473 " + serverName + " " + channelName + " :Cannot join channel (+i)\r\n";
}

std::string Replier::errBadChannelKey(const std::vector<std::string>& args)
{
	if (args.size() != 2)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errBadChannelKey()"));
	}

	const std::string& serverName = args[0];
	const std::string& channelName = args[1];

	return "475 " + serverName + " " + channelName + " :Cannot join channel (+k)\r\n";
}

std::string Replier::errBadChanMask(const std::vector<std::string>& args)
{
	if (args.size() != 2)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errBadChanMask()"));
	}

	const std::string& serverName = args[0];
	const std::string& channelName = args[1];

	return "476 " + serverName + " " + channelName + " :Bad Channel Mask\r\n";
}

std::string Replier::errChanOPrivsNeeded(const std::vector<std::string>& args)
{
	if (args.size() != 2)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errChanOprivsNeeded()"));
	}

	const std::string& serverName = args[0];
	const std::string& channelName = args[1];

	return "482 " + serverName + " " + channelName + " :You're not channel operator\r\n";
}
