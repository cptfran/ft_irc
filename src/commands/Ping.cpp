#include "commands/Ping.h"
#include "server/Server.h"
#include "utils/Utils.h"
#include "replier/Replier.h"

Ping::Ping()
{

}

Ping::~Ping()
{

}

void Ping::execute(Server& server, Client& requester, const std::vector<std::string>& args) const
{
	const std::string& serverToReply = args[0];

	if (args.empty())
	{
		Replier::reply(requester.getFd(), Replier::rplPong, Utils::anyToVec(server.getName()));
		return;
	}

	Replier::reply(requester.getFd(), Replier::rplPong, Utils::anyToVec(server.getName(), serverToReply));
}
