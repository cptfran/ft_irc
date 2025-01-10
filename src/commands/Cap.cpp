#include "commands/Cap.h"
#include "Server.h"
#include "Utils.h"
#include "Replier.h"

Cap::Cap()
{

}

Cap::~Cap()
{

}

void Cap::execute(Server& server, Client& client, const std::vector<std::string>& args) const
{
	if (!args.empty() && args[0] == "LS")
	{
		Replier::reply(client.getFd(), Replier::rplCap, Utils::anyToVec(server.getName()));
	}
}
