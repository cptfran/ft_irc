#include "commands/Nick.h"
#include "Server.h"

Nick::Nick()
{

}

Nick::~Nick()
{

}

void Nick::execute(Server& server, Client& client, const std::vector<std::string>& args) const
{
	(void)server;

	if (!args.empty())
	{
		server.handleNicknameCollision(args[0]);
		client.setNickname(args[0]);
	}
}
