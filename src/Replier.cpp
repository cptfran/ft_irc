#include "../include/Replier.h"
#include <sys/socket.h>
#include "Log.h"

Replier::Replier()
{

}

Replier::~Replier()
{

}

void Replier::reply(const Client& client, const ReplyFunction func, const std::vector<std::string>& args)
{
	const std::string reply = func(args);
	// DEBUG_LOG(reply);
	send(client.getFd(), reply.c_str(), reply.size(), 0);
}

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
	return "Pong :" + args[0] + " " + args[1] + "\r\n";
}

std::string Replier::rplCap(const std::vector<std::string>& args)
{
	(void)args;
	return ":server CAP * LS :\r\n";
}


std::string Replier::rplNoTopic(const std::vector<std::string>& args)
{
	if (args.size() != 1)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplNoTopic()"));
	}
	return "331 " + args[0] + " :No topic is set\r\n";
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

std::string Replier::errNotRegistered(const std::vector<std::string>& args)
{
	if (args.size() != 1)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errNotRegistered()"));
	}
	return "451 " + args[0] + " :You have not registered\r\n";
}

std::string Replier::errNeedMoreParams(const std::vector<std::string>& args)
{
	if (args.size() != 2)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errNeedMoreParams()"));
	}
	return "461 " + args[0] + " " + args[1] + " :Not enough parameters\r\n";
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
	if (args.size() != 1)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errChannelIsFull()"));
	}
	return "471 " + args[0] + " :Cannot join channel (+1)\r\n";
}