#include "../../include/commands/Cap.h"
#include "../../include/Server.h"
#include "../../include/Utils.h"

Cap::Cap()
{

}

Cap::~Cap()
{

}

void Cap::execute(const Server& server, Client& client, const std::vector<std::string>& args) const
{
	(void)server;
	if (!args.empty() && args[0] == "LS")
	{
		// TODO: in the end check if it should send any capabilities
		server.reply(client, Server::rplCap, Utils::anyToVec(std::string("")));
	}
}
