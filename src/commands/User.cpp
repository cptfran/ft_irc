#include "commands/User.h"
#include "core/Log.h"
#include "core/Server.h"
#include "utils/Utils.h"
#include "communication/Replier.h"
#include "data/Channel.h"
#include "communication/ClientTranslator.h"

User::User()
{

}

User::~User()
{

}

/**
 * @brief Executes the USER command which sets the username and realname for the client.
 *
 * This method validates the number of arguments and the format of the realname.
 * If the validation passes, it sets the username and realname for the client.
 * If the validation fails, it sends an Log::ERROR response to the client.
 *
 * @param serverManager Reference to the server's Manager object.
 * @param requester Reference to the Client object that issued the command.
 * @param args Vector of arguments passed with the command.
 */
void User::execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const
{
	ConfigManager& configManager = serverManager.getConfigManager();

	if (args.size() < 4)
	{
		Replier::addToQueue(requester.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(configManager.getName(),
			requester.getNickname(), std::string("USER")));
		return;
	}

	const std::string& realname = args[3];
	if (realname[0] != ':')
	{
		Replier::addToQueue(requester.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(configManager.getName(),
			requester.getNickname(), std::string("USER")));
		return;
	}

	std::string username = args[0];
	if (username.length() > ConfigManager::MAX_USERNAME_LEN)
	{
		username.resize(ConfigManager::MAX_USERNAME_LEN);
	}
	requester.setUsername(username);

	requester.setRealname(ClientTranslator::sanitizeColonMessage(realname));
}
