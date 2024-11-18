#include "../../include/commands/Ping.h"
#include "../../include/Server.h"
#include "../../include/Utils.h"

Ping::Ping()
{

}

Ping::~Ping()
{

}

void Ping::execute(const Server& server, Client& client, const std::vector<std::string>& args) const
{
	if (args.empty())
	{
		server.reply(client, Server::rplPong, Utils::anyToVec(server.getName()));
		return;
	}
	server.reply(client, Server::rplPong, Utils::anyToVec(server.getName(), args[0]));
}
