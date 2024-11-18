#include "../../include/commands/Nick.h"
#include "../../include/Server.h"

Nick::Nick()
{

}

Nick::~Nick()
{

}

void Nick::execute(const Server& server, Client& client, const std::vector<std::string>& args) const
{
	(void)server;
	if (!args.empty())
	{
		client.setNickname(args[0]);
	}
}
