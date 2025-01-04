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
	if (mask[0] == '#')
	{
		const Channel* channel = server.getChannel(mask);
		if (channel == NULL)
		{
			Replier::reply(client.getFd(), Replier::errNoSuchChannel, Utils::anyToVec(server.getName(),
				client.getNickname(), mask));
			return;
		}
		// :serverName 352 requestorNickname channel username hostname server nickname H@ :2 realname
		const std::vector<std::string>& userList = channel->getUserListForWhoQuery(server.getName());
		for (std::vector<std::string>::const_iterator it = userList.begin(); it != userList.end(); ++it)
		{
			Replier::reply(client.getFd(), Replier::rplWhoReply, Utils::anyToVec(server.getName(),
				client.getNickname(), *it));
		}
		Replier::reply(client.getFd(), Replier::rplEndOfWho, Utils::anyToVec(server.getName(),
			client.getNickname(), mask));
		return;
	}
}

// pattern = use* str = user
// pattern = se* str = ser
// pattern = e* str = er
// pattern = * str = r
// pattern = \0 str = r || pattern = * str = \0
// pattern = \0 str = \0

bool matchWildcard(const char* pattern, const char* str, bool bracket)
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
		return matchWildcard(pattern + 1, str, bracket) || (*str != '\0' && matchWildcard(pattern, str + 1, bracket));
	}

	if (*pattern == '[')
	{
		return matchWildcard()
	}

	// If the current character in the pattern is '?', it can match any single character in the string
	if (*pattern == '?' || *pattern == *str)
	{
		return matchWildcard(pattern + 1, str + 1, false);
	}

	// If the current characters in the pattern and the string do not match, it's not a match
	return false;
}

bool matchWildcard(const std::string& pattern, const std::string& str)
{
	return matchWildcard(pattern.c_str(), str.c_str());
}

