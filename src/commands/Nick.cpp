#include "commands/Nick.h"
#include "manager/Server.h"
#include "replier/Replier.h"
#include "utils/Utils.h"
#include "data/Channel.h"
#include "client/ClientTranslator.h"

Nick::Nick() : Command()
{

}

Nick::~Nick()
{

}

void Nick::execute(Server& server, Client& requester, const std::vector<std::string>& args) const
{
	if (args.empty())
	{
		Replier::addToQueue(requester.getFd(), Replier::errNoNicknameGiven, Utils::anyToVec(server.getName(),
			requester.getNickname()));
		return;
	}

	const std::string& nickname = args[0];

	if (!ClientTranslator::nicknameValid(nickname))
	{
		Replier::addToQueue(requester.getFd(), Replier::errOneusNickname, Utils::anyToVec(server.getName(),
			requester.getNickname(), nickname));
		return;
	}
	if (!requester.getNickname().empty())
	{
		if (server.findClientByNickname(nickname) != NULL)
		{
			Replier::addToQueue(requester.getFd(), Replier::errNicknameInUse, Utils::anyToVec(server.getName(),
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
