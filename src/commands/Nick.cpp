#include "commands/Nick.h"
#include "server/Server.h"
#include "replier/Replier.h"
#include "utils/Utils.h"
#include "channel/Channel.h"
#include "client/ClientTranslator.h"

Nick::Nick()
{

}

Nick::~Nick()
{

}

void Nick::execute(Server& server, Client& requester, const std::vector<std::string>& args) const
{
	if (args.empty())
	{
		Replier::reply(requester.getFd(), Replier::errNoNicknameGiven, Utils::anyToVec(server.getName(),
			requester.getNickname()));
		return;
	}

	const std::string& nickname = args[0];

	if (!ClientTranslator::nicknameValid(nickname))
	{
		Replier::reply(requester.getFd(), Replier::errOneusNickname, Utils::anyToVec(server.getName(),
			requester.getNickname(), nickname));
		return;
	}
	if (!requester.getNickname().empty())
	{
		if (server.findClientByNickname(nickname) != NULL)
		{
			Replier::reply(requester.getFd(), Replier::errNicknameInUse, Utils::anyToVec(server.getName(),
				requester.getNickname(), nickname));
			return;
		}
	}
	else
	{
		server.handleNicknameCollision(requester.getFd(), nickname);
	}
	requester.setNickname(args[0]);
}
