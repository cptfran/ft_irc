#include "../../include/commands/Join.h"
#include "../../include/Server.h"
#include "../../include/Utils.h"

Join::Join()
{

}

Join::~Join()
{

}

void Join::execute(const Server& server, Client& client, const std::vector<std::string>& args) const
{
	// TODO: handle this command properly. This is just a test code.
	server.reply(client, Server::rplNoTopic, Utils::anyToVec(args[0]));
}
