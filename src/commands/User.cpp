#include "commands/User.h"

#include "Log.h"
#include "Server.h"
#include "Utils.h"
#include "Replier.h"

User::User()
{

}

User::~User()
{

}

void User::execute(Server& server, Client& client, const std::vector<std::string>& args) const
{
	if (args.size() < 4)
	{
		Replier::reply(client.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(server.getName(),
			client.getNickname(), std::string("USER")));
	}

	const std::string& realname = args[3];
	if (realname[0] != ':')
	{
		Replier::reply(client.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(server.getName(),
			client.getNickname(), std::string("USER")));
	}

	const std::string& username = args[0];
	client.setUsername(username);

	client.setRealname(Utils::sanitizeColonMessage(realname));
}
