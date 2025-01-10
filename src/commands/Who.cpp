#include "commands/Who.h"
#include "Channel.h"
#include "Log.h"
#include "Replier.h"
#include "Server.h"
#include "Utils.h"
#include "ClientTranslator.h"

Who::Who()
{

}

Who::~Who()
{

}

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
		const bool operatorOnly = (args.size() == 2 && args[1] == "o");
		handleChannel(server, mask, client, operatorOnly);
		return;
	}

	// User prompting for users info.
	handleUsers(server, client, mask);
}

void Who::handleChannel(Server& server, const std::string& mask, Client& requester, const bool operatorOnly) const
{
	const Channel* channel = server.getChannel(mask);
	if (channel == NULL)
	{
		Replier::reply(requester.getFd(), Replier::errNoSuchChannel, Utils::anyToVec(server.getName(),
			requester.getNickname(), mask));
		return;
	}

	const std::vector<std::string>& userList = channel->getUserListForWhoQuery(server.getName(), operatorOnly);
	for (std::vector<std::string>::const_iterator it = userList.begin(); it != userList.end(); ++it)
	{
		Replier::reply(requester.getFd(), Replier::rplWhoReply, Utils::anyToVec(server.getName(),
			requester.getNickname(), *it));
	}
	Replier::reply(requester.getFd(), Replier::rplEndOfWho, Utils::anyToVec(server.getName(),
		requester.getNickname(), mask));
}


void Who::handleUsers(Server& server, Client& requester, const std::string& mask) const
{
	std::map<int, Client> users = server.getClients();

	for (std::map<int, Client>::iterator it = users.begin(); it != users.end(); ++it)
	{
		if ((mask != "0" && !userMatchesMask(server, it->second, mask)) ||
			(it->second.isInvisible() &&
			!server.usersHaveCommonChannel(requester.getNickname(), it->second.getNickname())))
		{
			continue;
		}

		const std::string sanitizedUserInfo = "* " + it->second.getUsername() + " " + it->second.getHostname() + " " +
			server.getName() + " " + it->second.getNickname() + " H :0 " + it->second.getRealname() + "\r\n";

		Replier::reply(requester.getFd(), Replier::rplWhoReply, Utils::anyToVec(server.getName(),
			requester.getNickname(), sanitizedUserInfo));
	}
	Replier::reply(requester.getFd(), Replier::rplEndOfWho, Utils::anyToVec(server.getName(),
		requester.getNickname(), mask));
}

bool Who::userMatchesMask(Server& server, Client& requester, const std::string& mask) const
{
	if (ClientTranslator::matchWildcard(mask.c_str(), requester.getHostname().c_str()) ||
		ClientTranslator::matchWildcard(mask.c_str(), server.getName().c_str()) ||
		ClientTranslator::matchWildcard(mask.c_str(), requester.getRealname().c_str()) ||
		ClientTranslator::matchWildcard(mask.c_str(), requester.getNickname().c_str()))
	{
		return true;
	}

	return false;
}