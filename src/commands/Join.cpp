#include "../../include/commands/Join.h"
#include "../../include/Server.h"
#include "../../include/Utils.h"
#include "../../include/Replier.h"

Join::Join()
{

}

Join::~Join()
{

}

void Join::execute(Server& server, Client& client, const std::vector<std::string>& args) const
{
	// TODO: handle this command properly. This is just a test code.
	if (!client.registered(server.getPassword()))
	{
		Replier::reply(client, Replier::errNotRegistered, Utils::anyToVec(std::string("")));
		return;
	}
	Replier::reply(client, Replier::rplNoTopic, Utils::anyToVec(args[0]));
}
