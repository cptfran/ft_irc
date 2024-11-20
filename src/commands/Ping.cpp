#include "../../include/commands/Ping.h"
#include "../../include/Server.h"
#include "../../include/Utils.h"
#include "../../include/Replier.h"

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
		Replier::reply(client, Replier::rplPong, Utils::anyToVec(server.getName()));
		return;
	}
	Replier::reply(client, Replier::rplPong, Utils::anyToVec(server.getName(), args[0]));
}
