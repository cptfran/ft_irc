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
		Replier::reply(client, Replier::errPasswdMismatch, Utils::anyToVec(server.getName()));
		throw std::invalid_argument(PASSWORD_EMPTY);
	}
	if (args[0] != server.getPassword())
	{
		Replier::reply(client, Replier::errPasswdMismatch, Utils::anyToVec(server.getName()));
		throw std::invalid_argument(WRONG_PASSWORD);
	}
	client.setPassword(args[0]);
}
