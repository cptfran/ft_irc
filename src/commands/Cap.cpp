#include "../../include/commands/Cap.h"
#include "../../include/Server.h"
#include "../../include/Utils.h"
#include "../../include/Replier.h"

Cap::Cap()
{

}

Cap::~Cap()
{

}

void Cap::execute(Server& server, Client& client, const std::vector<std::string>& args) const
{
	(void)server;
	if (!args.empty() && args[0] == "LS")
	{
		// TODO: in the end check if it should send any capabilities.
		Replier::reply(client, Replier::rplCap, Utils::anyToVec(std::string("")));
	}
}
