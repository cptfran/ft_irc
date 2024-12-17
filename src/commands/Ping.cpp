#include "commands/Ping.h"
#include "Server.h"
#include "Utils.h"
#include "Replier.h"

Ping::Ping()
{

}

Ping::~Ping()
{

}

void Ping::execute(Server& server, Client& client, const std::vector<std::string>& args) const
{
	const std::string& serverToReply = args[0];

	if (args.empty())
	{
		Replier::reply(client.getFd(), Replier::rplPong, Utils::anyToVec(server.getName()));
		return;
	}

	Replier::reply(client.getFd(), Replier::rplPong, Utils::anyToVec(server.getName(), serverToReply));
}
