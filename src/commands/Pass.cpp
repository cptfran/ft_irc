#include "commands/Pass.h"
#include "core/Server.h"
#include "utils/Utils.h"
#include "core/Log.h"
#include "communication/Replier.h"

Pass::Pass() : Command()
{

}

Pass::~Pass()
{

}

void Pass::execute(Server& server, Client& requester, const std::vector<std::string>& args) const
{
	if (args.empty())
	{
		Replier::addToQueue(requester.getFd(), Replier::errPasswdMismatch, Utils::anyToVec(server.getName(),
			requester.getNickname()));
		return;
	}

	const std::string& enteredPassword = args[0];
	if (enteredPassword != server.getPassword())
	{
		Replier::addToQueue(requester.getFd(), Replier::errPasswdMismatch, Utils::anyToVec(server.getName(),
			requester.getNickname()));
		return;
	}

	requester.setPassword(args[0]);
}
