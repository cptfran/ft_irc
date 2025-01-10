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
		const std::string& nickname = args[0];
		server.handleNicknameCollision(client.getFd(), nickname);
		client.setNickname(args[0]);
	}
}
