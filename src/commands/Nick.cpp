#include "commands/Nick.h"
#include "core/Server.h"
#include "communication/Replier.h"
#include "utils/Utils.h"
#include "data/Channel.h"
#include "communication/ClientTranslator.h"

Nick::Nick() : Command()
{

}

Nick::~Nick()
{

}

void Nick::execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const
{
	ConfigManager& configManager = serverManager.getConfigManager();

	if (args.empty())
	{
		Replier::addToQueue(requester.getFd(), Replier::errNoNicknameGiven, Utils::anyToVec(configManager.getName(),
			requester.getNickname()));
		return;
	}

	const std::string& nickname = args[0];

	if (!ClientTranslator::nicknameValid(nickname))
	{
		Replier::addToQueue(requester.getFd(), Replier::errOneusNickname, Utils::anyToVec(configManager.getName(),
			requester.getNickname(), nickname));
		return;
	}
	if (!requester.getNickname().empty())
	{
		if (serverManager.getClientManager().getClientByNickname(nickname) != NULL)
		{
			Replier::addToQueue(requester.getFd(), Replier::errNicknameInUse, Utils::anyToVec(configManager.getName(),
				requester.getNickname(), nickname));
			return;
		}
	}
	else
	{
		serverManager.handleNicknameCollision(requester.getFd(), nickname);
	}
	requester.setNickname(args[0]);
}
