#include "commands/Who.h"

#include "Channel.h"
#include "Log.h"
#include "Replier.h"
#include "Server.h"
#include "Utils.h"

// Command: WHO
//  Parameters: [ <mask> [ "o" ] ]
//
//  The WHO command is used by a client to generate a query which returns
//  a list of information which 'matches' the <mask> parameter given by
//  the client.  In the absence of the <mask> parameter, all visible
//  (users who aren't invisible (user mode +i) and who don't have a
//  common channel with the requesting client) are listed.  The same
//  result can be achieved by using a <mask> of "0" or any wildcard which
//  will end up matching every visible user.
//
//  The <mask> passed to WHO is matched against users' host, server, real
//  name and nickname if the channel <mask> cannot be found.
//
// RFC 2812          Internet Relay Chat: Client Protocol        April 2000
//
//
//  If the "o" parameter is passed only operators are returned according
//  to the <mask> supplied.
//
//  Numeric Replies:
//
// 		 ERR_NOSUCHSERVER
// 		 RPL_WHOREPLY                  RPL_ENDOFWHO
//
//  Examples:
//
//  WHO *.fi                        ; Command to list all users who match
// 								 against "*.fi".
//
//  WHO jto* o                      ; Command to list all users with a
// 								 match against "jto*" if they are an
// 								 operator.
Who::Who()
{

}

Who::~Who()
{

}

// TODO: need to implement (maybeee).
void Who::execute(Server& server, Client& client, const std::vector<std::string>& args) const
{
	// Not enough parameters.
	if (args.empty())
	{
		Replier::reply(client.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(server.getName(),                   
			client.getNickname(), std::string("WHO")));
		return;
	}

	const std::string& mask = args[0];

	// User prompting for channel info.
	if (mask[0] == '#')
	{
		handleChannel(server, mask, client);
		return;
	}

	// User prompting for users info.
	handleUsers(server, client, mask);
}

void Who::handleChannel(Server& server, const std::string& mask, Client& requester) const
{
	const Channel* channel = server.getChannel(mask);
	if (channel == NULL)
	{
		Replier::reply(requester.getFd(), Replier::errNoSuchChannel, Utils::anyToVec(server.getName(),
			requester.getNickname(), mask));
		return;
	}
	// :serverName 352 requestorNickname channel username hostname server nickname H@ :2 realname
	const std::vector<std::string>& userList = channel->getUserListForWhoQuery(server.getName());
	for (std::vector<std::string>::const_iterator it = userList.begin(); it != userList.end(); ++it)
	{
		Replier::reply(requester.getFd(), Replier::rplWhoReply, Utils::anyToVec(server.getName(),
			requester.getNickname(), *it));
	}
	Replier::reply(requester.getFd(), Replier::rplEndOfWho, Utils::anyToVec(server.getName(),
		requester.getNickname(), mask));
}


// host, server, realname and nickname
void Who::handleUsers(Server& server, Client& requester, const std::string& mask) const
{
	std::map<int, Client> users = server.getClients();

	for (std::map<int, Client>::iterator it = users.begin(); it != users.end(); ++it)
	{
		if (!userMatchesMask(server, it->second, mask) ||
			(it->second.isInvisible() &&
			!server.usersHaveCommonChannel(requester.getNickname(), it->second.getNickname())))
		{
			continue;
		}

		const std::string sanitizedUserInfo = it->second.getUsername() + " " + it->second.getHostname() + " " +
			server.getName() + " " + it->second.getNickname() + " H :0 " + it->second.getRealname() + "\r\n";

		Replier::reply(requester.getFd(), Replier::rplWhoReply, Utils::anyToVec(server.getName(),
			requester.getNickname(), sanitizedUserInfo));
	}
	Replier::reply(requester.getFd(), Replier::rplEndOfWho, Utils::anyToVec(server.getName(),
		requester.getNickname(), mask));
}

bool Who::userMatchesMask(Server& server, Client& requester, const std::string& mask) const
{
	if (matchWildcard(mask.c_str(), requester.getHostname().c_str()) ||
		matchWildcard(mask.c_str(), server.getName().c_str()) ||
		matchWildcard(mask.c_str(), requester.getRealname().c_str()) ||
		matchWildcard(mask.c_str(), requester.getNickname().c_str()))
	{
		return true;
	}

	return false;
}

bool Who::matchWildcard(const char* pattern, const char* str) const
{
	// Base case: if we reach the end of both the pattern and the string, it's a match
	if (*pattern == '\0' && *str == '\0')
	{
		return true;
	}

	// If the current character in the pattern is '*', it can match zero or more characters in the string
	if (*pattern == '*')
	{
		// Try to match the rest of the pattern with the current string or the rest of the string
		return matchWildcard(pattern + 1, str) || (*str != '\0' && matchWildcard(pattern, str + 1));
	}

	// If the current character in the pattern is '?', it can match any single character in the string
	if (*pattern == '?' || *pattern == *str)
	{
		return matchWildcard(pattern + 1, str + 1);
	}

	// If the current characters in the pattern and the string do not match, it's not a match
	return false;
}