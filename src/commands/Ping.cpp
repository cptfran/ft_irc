#include "commands/Ping.h"
#include "core/Server.h"
#include "utils/Utils.h"
#include "communication/Replier.h"

Ping::Ping() : Command()
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
		Replier::addToQueue(requester.getFd(), Replier::rplPong, Utils::anyToVec(server.getName()));
		return;
	}

	Replier::addToQueue(requester.getFd(), Replier::rplPong, Utils::anyToVec(server.getName(), serverToReply));
}
