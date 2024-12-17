#include "commands/Pass.h"
#include "Server.h"
#include "Utils.h"
#include "Log.h"
#include "Replier.h"

Pass::Pass()
{

}

Pass::~Pass()
{

}

void Pass::execute(Server& server, Client& client, const std::vector<std::string>& args) const
{
	if (args.empty())
	{
		Replier::reply(client.getFd(), Replier::errPasswdMismatch, Utils::anyToVec(server.getName()));
		return;
	}

	const std::string& enteredPassword = args[0];
	if (enteredPassword != server.getPassword())
	{
		Replier::reply(client.getFd(), Replier::errPasswdMismatch, Utils::anyToVec(server.getName()));
		return;
	}

	client.setPassword(args[0]);
}
