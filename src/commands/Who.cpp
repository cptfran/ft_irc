#include "commands/Who.h"
#include "data/Channel.h"
#include "core/Log.h"
#include "communication/Replier.h"
#include "core/Server.h"
#include "utils/Utils.h"
#include "communication/ClientTranslator.h"

Who::Who() : Command()
{

}

Who::~Who()
{

}

void Who::execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const
{
	// Not enough parameters.
	if (args.empty())
	{
		Replier::addToQueue(requester.getFd(), Replier::errNeedMoreParams, 
			Utils::anyToVec(serverManager.getConfigManager().getName(), requester.getNickname(), std::string("WHO")));
		return;
	}

	const std::string& mask = args[0];

	// User prompting for channel info.
	if (mask[0] == '#')
	{
		bool operatorOnly = (args.size() == 2 && args[1] == "o");
		handleChannel(serverManager, mask, requester, operatorOnly);
		return;
	}

	// User prompting for users info.
	handleUsers(serverManager, requester, mask);
}

void Who::handleChannel(Manager& serverManager, const std::string& mask, Client& requester, bool operatorOnly) const
{
	ConfigManager& configManager = serverManager.getConfigManager();

	const Channel* channel = serverManager.getChannelManager().getChannel(mask);
	if (channel == NULL)
	{
		Replier::addToQueue(requester.getFd(), Replier::errNoSuchChannel, Utils::anyToVec(configManager.getName(),
			requester.getNickname(), mask));
		return;
	}

	const std::vector<std::string>& userList = channel->getUserListForWhoQuery(configManager.getName(), operatorOnly);
	for (std::vector<std::string>::const_iterator it = userList.begin(); it != userList.end(); ++it)
	{
		Replier::addToQueue(requester.getFd(), Replier::rplWhoReply, Utils::anyToVec(configManager.getName(),
			requester.getNickname(), *it));
	}
	Replier::addToQueue(requester.getFd(), Replier::rplEndOfWho, Utils::anyToVec(configManager.getName(),
		requester.getNickname(), mask));
}


void Who::handleUsers(Manager& serverManager, Client& requester, const std::string& mask) const
{
	ConfigManager& configManager = serverManager.getConfigManager();

	std::map<int, Client> users = serverManager.getClientManager().getClients();
	for (std::map<int, Client>::iterator it = users.begin(); it != users.end(); ++it)
	{
		if ((mask != "0" && !userMatchesMask(configManager.getName(), it->second, mask)) ||
			(it->second.isInvisible() &&
			!serverManager.getChannelManager().usersHaveCommonChannel(requester.getNickname(), 
				it->second.getNickname())))
		{
			continue;
		}

		const std::string sanitizedUserInfo = "* " + it->second.getUsername() + " " + it->second.getHostname() + " " +
			configManager.getName() + " " + it->second.getNickname() + " H :0 " + it->second.getRealname() + "\r\n";

		Replier::addToQueue(requester.getFd(), Replier::rplWhoReply, Utils::anyToVec(configManager.getName(),
			requester.getNickname(), sanitizedUserInfo));
	}
	Replier::addToQueue(requester.getFd(), Replier::rplEndOfWho, Utils::anyToVec(configManager.getName(),
		requester.getNickname(), mask));
}

bool Who::userMatchesMask(const std::string& serverName, Client& requester, const std::string& mask) const
{
	if (ClientTranslator::matchWildcard(mask.c_str(), requester.getHostname().c_str()) ||
		ClientTranslator::matchWildcard(mask.c_str(), serverName.c_str()) ||
		ClientTranslator::matchWildcard(mask.c_str(), requester.getRealname().c_str()) ||
		ClientTranslator::matchWildcard(mask.c_str(), requester.getNickname().c_str()))
	{
		return true;
	}

	return false;
}