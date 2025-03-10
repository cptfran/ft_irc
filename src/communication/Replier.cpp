#include "communication/Replier.h"
#include <sys/socket.h>
#include "core/Log.h"
#include <map>

std::map<int, std::vector<std::string> > Replier::rplQueue;

Replier::Replier()
{

}

Replier::~Replier()
{

}

/**
* @brief Adds a reply to the queue for the given file descriptor.
*
* This function takes a file descriptor, a reply function, and its arguments,
* then adds the result of the reply function to the queue associated with the file descriptor.
*
* @param fd The file descriptor of the client.
* @param func The reply function to generate the reply.
* @param funcArgs The arguments to pass to the reply function.
*/
void Replier::addToQueue(const int fd, const ReplyFunction func, const std::vector<std::string>& funcArgs)
{
    
    rplQueue[fd].push_back(func(funcArgs));
}

/**
* @brief Checks if a client is in the queue.
*
* This function checks if there is an entry in the queue for the given file descriptor.
*
* @param fd The file descriptor of the client.
* @return true if the client is in the queue, false otherwise.
*/
bool Replier::clientInQueue(const int fd)
{
    if (Replier::rplQueue.find(fd) == Replier::rplQueue.end())
    {
        return false;
    }

    return true;
}

/**
* @brief Sends the first reply from the queue for the given file descriptor.
*
* This function finds the queue associated with the given file descriptor,
* sends the first reply in the queue, and then removes it from the queue.
* If the queue becomes empty, the entry is removed from the map.
*
* @param fd The file descriptor of the client.
*/
// first (fd = 4) second ("first reply" "second reply")
void Replier::sendFromQueue(const int fd)
{
    
    // Find the vector associated with the given file descriptor.
    std::map<int, std::vector<std::string> >::iterator it = Replier::rplQueue.find(fd);
    if (it == Replier::rplQueue.end())
    {
        return;
    }

    // Get the first reply from the vector.
    const std::string& reply = it->second.front();
    send(fd, reply.c_str(), reply.size(), 0);

    // Erase the first element from the vector.
    it->second.erase(it->second.begin());

    // If the vector becomes empty, erase the element from the map.
    if (it->second.empty())
    {
        Replier::rplQueue.erase(it);
    }
}

std::string Replier::rplWelcome(const std::vector<std::string>& args)
{
	if (args.size() != 4)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("rplWelcome()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& username = args[2];
	const std::string& hostname = args[3];

	return ":" + serverName + " 001 " + nickname + " :Welcome to the Internet Relay Network " + nickname + "!"
	+ username + "@" + hostname + "\r\n";
}

std::string Replier::rplYourHost(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("rplYourHost()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& version = args[2];

	return ":" + serverName + " 002 " + nickname + " :Your host is " + serverName + ", running version " + version +
		"\r\n";
}

std::string Replier::rplCreated(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("rplCreated()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& creationDate = args[2];

	return ":" + serverName + " 003 " + nickname + " :This server was created " + creationDate + "\r\n";
}

std::string Replier::rplMyInfo(const std::vector<std::string>& args)
{
	if (args.size() != 5)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("rplMyInfo()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& version = args[2];
	const std::string& availableUserModes = args[3];
	const std::string& availableChannelModes = args[4];

	return ":" + serverName + " 004 " + nickname + " :" + nickname + " Version: " + version + 
		" Available user modes: " + availableUserModes + " Available channel modes: " + availableChannelModes + "\r\n";
}

std::string Replier::rplPong(const std::vector<std::string>& args)
{
	if (args.empty())
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("rplPong()"));
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
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("rplCap()"));
	}

	const std::string& serverName = args[0];

	// Here if asked for capabilities, server sends empty list, because based on the project subject we don't implement
	// fully any known capability.
	return ":" + serverName + ": CAP * LS :\r\n";
}

std::string Replier::rplKick(const std::vector<std::string>& args)
{
	if (args.size() < 5)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("rplKick()"));
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

std::string Replier::rplUModeIs(const std::vector<std::string>& args)
{
	if (args.size() < 2)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("rplUModeIs()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];

	if (args.size() == 3)
	{
		const std::string& modes = args[2];
		return ":" + serverName + " 221 " + nickname + " " + modes + "\r\n";
	}

	return ":" + serverName + " 221 " + nickname + "\r\n";
}

std::string Replier::rplEndOfWho(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("rplEndOfWho()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& mask = args[2];

	return ":" + serverName + " 315 " + nickname + " " + mask + " :End of WHO list\r\n";
}

std::string Replier::rplChannelModeIs(const std::vector<std::string>& args)
{
	if (args.size() < 2)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("rplChannelModeIs()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& channelName = args[2];
	std::string modeParams;
	for (size_t i = 3; i < args.size(); ++i)
	{
		modeParams += args[i] + " ";
		if (i + 1 == args.size())
		{
			modeParams = modeParams.substr(0, modeParams.length() - 1);
		}
	}

	return ":" + serverName + " 324 " + nickname + " " + channelName + " " + modeParams + "\r\n";
}

std::string Replier::rplNoTopic(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("rplNoTopic()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& channelName = args[2];

	return ":" + serverName + " 331 " + nickname + " " + channelName + " :No topic is set\r\n";
}

std::string Replier::rplTopic(const std::vector<std::string>& args)
{
	if (args.size() != 4)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("rplTopic()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& channelName = args[2];
	const std::string& topic = args[3];

	return ":" + serverName + " 332 " + nickname + " " + channelName + " :" + topic + "\r\n";
}

std::string Replier::rplInviting(const std::vector<std::string>& args)
{
	if (args.size() != 4)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("rplInviting()"));
	}

	const std::string& serverName = args[0];
	const std::string& invitingNickname = args[1];
	const std::string& channelName = args[2];
	const std::string& invitedNickname = args[3];

	return ":" + serverName + " 341 " + invitingNickname + " " + channelName + " " + invitedNickname + "\r\n";
}

std::string Replier::rplInvite(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("rplInvite()"));
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
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("rplJoin()"));
	}

	const std::string& nickname = args[0];
	const std::string& username = args[1];
	const std::string& hostname = args[2];
	const std::string& channelName = args[3];

	return ":" + nickname + "!" + username + "@" + hostname + " JOIN :" + channelName + "\r\n";
}

std::string Replier::rplWhoReply(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("rplWhoReply()"));
	}

	const std::string& serverName = args[0];
	const std::string& requestorNickname = args[1];
	const std::string& sanitizedUserInfo = args[2];

	return ":" + serverName + " 352 " + requestorNickname + " " + sanitizedUserInfo;
}

std::string Replier::rplNamReply(const std::vector<std::string>& args)
{
	if (args.size() < 4)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("rplNamReply()"));
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

	return reply;
}

std::string Replier::rplEndOfNames(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("rplEndOfNames()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& channelName = args[2];

	return ":" + serverName + " 366 " + nickname + " " + channelName + " :End of /NAMES list\r\n";
}

std::string Replier::rplEndOfBanList(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("rplEndOfBanList()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& channelName = args[2];

	return ":" + serverName + " 368 " + nickname + " " + channelName + " :End of channel ban list\r\n";
}

std::string Replier::rplPrivmsg(const std::vector<std::string>& args)
{
	if (args.size() != 5)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("rplPrivmsg()"));
	}

	const std::string& senderNickname = args[0];
	const std::string& senderUsername = args[1];
	const std::string& senderHostname = args[2];
	const std::string& targetNickname = args[3];
	const std::string& message = args[4];

	return ":" + senderNickname + "!" + senderUsername + "@" + senderHostname + " PRIVMSG " + targetNickname + " :" +
		message + "\r\n";
}

std::string Replier::errNoSuchNick(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("rplNoSuchNick()"));
	}

	const std::string& serverName = args[0];
	const std::string& requestingNickname = args[1];
	const std::string& target = args[2];

	return ":" + serverName + " 401 " + requestingNickname + " " + target + " :No such nick/channel\r\n";
}

std::string Replier::errNoSuchChannel(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errNoSuchChannel()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& channelName = args[2];

	return ":" + serverName + " 403 " + nickname + " " + channelName + " :No such channel\r\n";
}

std::string Replier::errCannotSendToChan(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errCannotSendToChan()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& channelName = args[2];

	return ":" + serverName + " 404 " + nickname + " " + channelName + " :Cannot send to channel\r\n";
}

std::string Replier::errTooManyChannels(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errTooManyChannels()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& channelName = args[2];

	return ":" + serverName + " 405 " + nickname + " " + channelName + " :You have joined too many channels\r\n";
}

std::string Replier::errTooManyTargets(const std::vector<std::string>& args)
{
	if (args.size() != 5)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errTooManyTargets()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& target = args[2];
	const std::string& errorCode = args[3];
	const std::string& abortMessage = args[4];

	return ":" + serverName + " 407 " + nickname + " " + target + " :" + errorCode + " " + abortMessage + "\r\n";
}

std::string Replier::errNoRecipient(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errNoRecipient()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& command = args[2];

	return ":" + serverName + " 411 " + nickname + " " + " :No recipient given " + command + "\r\n";
}

std::string Replier::errNoTextToSend(const std::vector<std::string>& args)
{
	if (args.size() != 2)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errNoTextToSend()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];


	return ":" + serverName + " 412 " + nickname + " " + " :No text to send\r\n";
}

std::string Replier::errUnknownCommand(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errUnknownCommand()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& command = args[2];

	return ":" + serverName + " 421 " + nickname + " " + command + " :Unknown command\r\n";
}

std::string Replier::errNoNicknameGiven(const std::vector<std::string>& args)
{
	if (args.size() != 2)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errNoNicknameGiven()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];

	return ":" + serverName + " 431 " + nickname + " :No nickname given\r\n";
}

std::string Replier::errOneusNickname(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errOneusNickname()"));
	}

	const std::string& serverName = args[0];
	const std::string& currNickname = args[1];
	const std::string& newNickname = args[1];

	return ":" + serverName + " 431 " + currNickname + " " + newNickname + " :Erroneous nickname\r\n";
}

std::string Replier::errNicknameInUse(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errNicknameInUse()"));
	}

	const std::string& serverName = args[0];
	const std::string& currNickname = args[1];
	const std::string& newNickname = args[2];

	return ":" + serverName + " 433 " + currNickname + " " + newNickname + " :Nickname is already in use\r\n";
}

std::string Replier::errNickCollision(const std::vector<std::string>& args)
{
	if (args.size() != 4)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errNickCollision()"));
	}

	const std::string& serverName = args[0];
	const std::string& joiningNickname = args[1];
	const std::string& existingUsername = args[2];
	const std::string& existingHostname = args[3];

	return ":" + serverName + " 436 " + joiningNickname + " :Nickname collision KILL from " + existingUsername + "@" +
		existingHostname + "\r\n";
}

std::string Replier::errUserNotInChannel(const std::vector<std::string>& args)
{
	if (args.size() != 4)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errUserNotInChannel()"));
	}

	const std::string& serverName = args[0];
	const std::string& nicknameKicker = args[1];
	const std::string& nicknameToKick = args[2];
	const std::string& channelName = args[3];

	return ":" + serverName + " 441 " + nicknameKicker + " " + nicknameToKick + " " + channelName +
		" :They aren't on that channel\r\n";
}

std::string Replier::errNotOnChannel(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errNotOnChannel()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& channelName = args[2];

	return ":" + serverName + " 442 " + nickname + " " + channelName + " :You're not on that channel\r\n";
}

std::string Replier::errUserOnChannel(const std::vector<std::string>& args)
{
	if (args.size() != 4)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errUserOnChannel()"));
	}
	const std::string& serverName = args[0];
	const std::string& nicknameInviting = args[1];
	const std::string& nicknameToInvite = args[2];
	const std::string& channelName = args[3];

	return ":" + serverName + " 443 " + nicknameInviting + " " + nicknameToInvite + " " + channelName +
		" :is already on channel\r\n";
}

std::string Replier::errNotRegistered(const std::vector<std::string>& args)
{
	if (args.size() != 2)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errNotRegistered()"));
	}

	const std::string& serverName = args[0];
	const std::string& username = args[1];

	return ":" + serverName + " 451 " + username + " :You have not registered\r\n";
}

std::string Replier::errNeedMoreParams(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errNeedMoreParams()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& command = args[2];

	return ":" + serverName + " 461 " + nickname + " " + command + " :Not enough parameters\r\n";
}


std::string Replier::errPasswdMismatch(const std::vector<std::string>& args)
{
	if (args.size() != 2)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errPasswdMismatch()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];

	return ":" + serverName + " 464 " + nickname + " :Password incorrect\r\n";
}

std::string Replier::errKeySet(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errKeySet()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& channelName = args[2];

	return ":" + serverName + " 467 " + nickname + " " + channelName + " :Channel key already set \r\n";
}

std::string Replier::errChannelIsFull(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errChannelIsFull()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& channelName = args[2];

	return ":" + serverName + " 471 " + nickname + " " + channelName + " :Cannot join channel (+1)\r\n";
}

std::string Replier::errUnknownMode(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errUnknownMode()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& modeChar = args[2];
	return ":" + serverName + " 472 " + nickname + " " + modeChar + " :is uknown mode char to me\r\n";
}

std::string Replier::errInviteOnlyChan(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errInviteOnlyChan()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& channelName = args[2];

	return ":" + serverName + " 473 " + nickname + " " + channelName + " :Cannot join channel (+i)\r\n";
}

std::string Replier::errBadChannelKey(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errBadChannelKey()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& channelName = args[2];

	return ":" + serverName + " 475 " + nickname + " " + channelName + " :Cannot join channel (+k)\r\n";
}

std::string Replier::errBadChanMask(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errBadChanMask()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& channelName = args[2];

	return ":" + serverName + " 476 " + nickname + " " + channelName + " :Bad Channel Mask\r\n";
}

std::string Replier::errChanOPrivsNeeded(const std::vector<std::string>& args)
{
	if (args.size() != 3)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errChanOprivsNeeded()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];
	const std::string& channelName = args[2];

	return ":" + serverName + " 482 " + nickname + " " + channelName + " :You're not channel operator\r\n";
}

std::string Replier::errUModeUnknownFlag(const std::vector<std::string>& args)
{
	if (args.size() != 2)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errClosingLink()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];

	return ":" + serverName + " 501 " + nickname + " :Unknown MODE flag\r\n";
}

std::string Replier::errUsersDontMatch(const std::vector<std::string>& args)
{
	if (args.size() != 2)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errUsersDontMatch()"));
	}

	const std::string& serverName = args[0];
	const std::string& nickname = args[1];

	return ":" + serverName + " 502 " + nickname + " :Cannot change mode for other users\r\n";
}

std::string Replier::errClosingLink(const std::vector<std::string>& args)
{
	if (args.size() != 2)
	{
		throw std::invalid_argument(Log::ERROR + Log::RPL_WRONG_NUM_OF_ARGS("errClosingLink()"));
	}

	const std::string& nickname = args[0];
	const std::string& hostname = args[1];

	return "Log::ERROR :Closing Link: " + nickname + "[" + hostname + "] (Registration timeout)\r\n";
}
