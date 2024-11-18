#include "../../include/commands/Pass.h"
#include "../../include/Server.h"
#include "../../include/Utils.h"
#include "../../include/Log.h"

Pass::Pass()
{

}

Pass::~Pass()
{

}

void Pass::execute(const Server& server, Client& client, const std::vector<std::string>& args) const
{
	if (args.empty())
	{
		server.reply(client, Server::errPasswdMismatch, Utils::anyToVec(server.getName()));
		throw std::invalid_argument(PASSWORD_EMPTY);
	}
	if (args[0] != server.getPassword())
	{
		server.reply(client, Server::errPasswdMismatch, Utils::anyToVec(server.getName()));
		throw std::invalid_argument(WRONG_PASSWORD);
	}
	client.setPassword(args[0]);
}
