#include "../include/Replier.h"
#include <sys/socket.h>
#include "Log.h"

Replier::Replier()
{

}

Replier::~Replier()
{

}

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


// TODO: add colon before every serverName as per protocol requires and check if it's still working as it should.
std::string Replier::rplWelcome(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplWelcome()"));
	}
	return ":@" + args[0] + " 001 " + args[1] + " :Welcome to the Internet Relay Network " + args[1] + "!" + args[2]
	+ "@" + args[0] + "\r\n";
}

std::string Replier::rplYourHost(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplYourHost()"));
	}
	return ":@" + args[0] + " 002 " + args[1] + " :Your host is " + args[0] + ", running version " + args[2] + "\r\n";
}

std::string Replier::rplCreated(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplCreated()"));
	}
	return ":@" + args[0] + " 003 " + args[1] + ": This server was created " + args[2] + "\r\n";
}

std::string Replier::rplMyInfo(const std::vector<std::string>& args)
{
	if (args.size() != 4)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplMyInfo()"));
	}
	return ":@" + args[0] + " 004 " + args[0] + " " + args[1] + " " + args[2] + " " + args[3] + "\r\n";
}

std::string Replier::rplPong(const std::vector<std::string>& args)
{
	if (args.empty())
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplPong()"));
	}
	if (args.size() == 1)
	{
		return "PONG :" + args[0] + "\r\n";
	}
	return "PONG :" + args[0] + " " + args[1] + "\r\n";
}

std::string Replier::rplCap(const std::vector<std::string>& args)
{
	(void)args;
	return ":server CAP * LS :\r\n";
}

std::string Replier::rplKick(const std::vector<std::string>& args)
{
	if (args.size() < 3)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplKick()"));
	}

	const std::string& kickerUser = args[0];
	const std::string& kickedUser = args[1];
	const std::string& channelName = args[2];

	if (args.size() == 4)
	{
		const std::string& comment = args[3].substr(1, args[3].length() - 1);
		return ":" + kickerUser + " KICK " + channelName + " " + kickedUser + " :" + comment + "\r\n";
	}

	return ":" + kickerUser + " KICK " + channelName + " " + kickedUser + "\r\n";
}

std::string Replier::rplNoTopic(const std::vector<std::string>& args)
{
	if (args.size() != 2)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplNoTopic()"));
	}

	const std::string& serverName = args[0];
	const std::string& channelName = args[1];

	return "331 " + serverName + " " + channelName + " :No topic is set\r\n";
}

std::string Replier::rplTopic(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplTopic()"));
	}

	const std::string& serverName = args[0];
	const std::string& channelName = args[1];
	const std::string& topic = args[2];

	return "332 " + serverName + " " + channelName + " :" + topic + "\r\n";
}


std::string Replier::rplNamReply(const std::vector<std::string>& args)
{
	if (args.size() < 3)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplNamReply()"));
	}

	const std::string& serverName = args[0];
	const std::string& channelName = args[1];
	std::vector<std::string> channelNicknames;
	for (size_t i = 2; i < args.size(); ++i)
	{
		channelNicknames.push_back(args[i]);
	}

	std::string reply = "353 " + serverName + " " + channelName + " :";
	for (size_t i = 0; i < channelNicknames.size(); ++i)
	{
		reply += channelNicknames[i] + " ";
	}
	reply += "\r\n";

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

	return "405 " + serverName + " " + channelName + " : You have joined too many channels\r\n";
}

std::string Replier::errUnknownCommand(const std::vector<std::string>& args)
{
	if (args.size() != 1)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errUnknownCommand()"));
	}

	return "421 " + args[0] + " :Unknown command\r\n";
}

std::string Replier::errNickCollision(const std::vector<std::string>& args)
{
	if (args.size() != 1)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errNickCollision()"));
	}

	return "436 " + args[0] + " :Nickname collision KILL\r\n";
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

	return "464 " + args[0] + " :Password incorrect.\r\n";
}

std::string Replier::errChannelIsFull(const std::vector<std::string>& args)
{
	if (args.size() != 2)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errChannelIsFull()"));
	}

	const std::string serverName = args[0];
	const std::string channelName = args[1];

	return "471 " + serverName + " " + channelName + " :Cannot join channel (+1)\r\n";
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
