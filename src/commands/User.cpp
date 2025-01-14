#include "commands/User.h"
#include "server/Log.h"
#include "server/Server.h"
#include "utils/Utils.h"
#include "replier/Replier.h"
#include "channel/Channel.h"
#include "client/ClientTranslator.h"

#define MAX_USERNAME_LEN 9

User::User()
{

}

User::~User()
{

}

// TODO: segfault when joining 'chan', problems when providing 'user user adsg :asdgdg' ?
void User::execute(Server& server, Client& requester, const std::vector<std::string>& args) const
{
	if (args.size() < 4)
	{
		Replier::reply(requester.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(server.getName(),
			requester.getNickname(), std::string("USER")));
	}

	const std::string& realname = args[3];
	if (realname[0] != ':')
	{
		Replier::reply(requester.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(server.getName(),
			requester.getNickname(), std::string("USER")));
	}

	std::string username = args[0];
	if (username.length() > MAX_USERNAME_LEN)
	{
		username.resize(MAX_USERNAME_LEN);
	}
	requester.setUsername(username);

	requester.setRealname(ClientTranslator::sanitizeColonMessage(realname));
}
