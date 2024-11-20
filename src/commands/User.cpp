#include "../../include/commands/User.h"

#include "Log.h"
#include "../../include/Server.h"
#include "../../include/Utils.h"
#include "../../include/Replier.h"

User::User()
{

}

User::~User()
{

}

void User::execute(Server& server, Client& client, const std::vector<std::string>& args) const
{
	if (args.size() < 4 || args[3][0] != ':')
	{
		Replier::reply(client, Replier::errNeedMoreParams, Utils::anyToVec(server.getName(),
			std::string("USER")));
	}
	client.setUsername(args[0]);
	client.setRealname(args[3].substr(1));
}
