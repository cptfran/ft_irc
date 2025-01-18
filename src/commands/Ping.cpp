#include "commands/Ping.h"
#include "core/Server.h"
#include "utils/Utils.h"
#include "communication/Replier.h"

Ping::Ping() : Command()
{

}

Ping::~Ping()
{

}

void Ping::execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const
{
	ConfigManager& configManager = serverManager.getConfigManager();

	const std::string& serverToReply = args[0];

	if (args.empty())
	{
		Replier::addToQueue(requester.getFd(), Replier::rplPong, Utils::anyToVec(configManager.getName()));
		return;
	}

	Replier::addToQueue(requester.getFd(), Replier::rplPong, Utils::anyToVec(configManager.getName(), serverToReply));
}
