#include "../../include/commands/User.h"
#include "../../include/Server.h"
#include "../../include/Utils.h"

User::User()
{

}

User::~User()
{

}

void User::execute(const Server& server, Client& client, const std::vector<std::string>& args) const
{
	(void)server;
	// TODO: if less than 5 tokens, not enough information, return something to client.

	client.setUsername(args[0]);

	// TODO: handle realname.
	// It must be noted that realname parameter must be the last parameter,
	// because it may contain space characters and must be prefixed with a
	// colon (':') to make sure this is recognised as such.
}
