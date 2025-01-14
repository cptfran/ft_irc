#include "commands/Pass.h"
#include "server/Server.h"
#include "utils/Utils.h"
#include "server/Log.h"
#include "replier/Replier.h"

Pass::Pass()
{

}

Pass::~Pass()
{

}

void Pass::execute(Server& server, Client& requester, const std::vector<std::string>& args) const
{
	if (args.empty())
	{
		Replier::reply(requester.getFd(), Replier::errPasswdMismatch, Utils::anyToVec(server.getName(),
			requester.getNickname()));
		return;
	}

	const std::string& enteredPassword = args[0];
	if (enteredPassword != server.getPassword())
	{
		Replier::reply(requester.getFd(), Replier::errPasswdMismatch, Utils::anyToVec(server.getName(),
			requester.getNickname()));
		return;
	}

	requester.setPassword(args[0]);
}
