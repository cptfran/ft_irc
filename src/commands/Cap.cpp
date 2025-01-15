#include "commands/Cap.h"
#include "server/Server.h"
#include "utils/Utils.h"
#include "replier/Replier.h"

Cap::Cap()
{

}

Cap::~Cap()
{

}

void Cap::execute(Server& server, Client& requester, const std::vector<std::string>& args) const
{
	if (!args.empty() && args[0] == "LS")
	{
		Replier::addToQueue(requester.getFd(), Replier::rplCap, Utils::anyToVec(server.getName()));
	}
}
