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

/**
 * @brief Executes the Nick command to change the client's nickname.
 * 
 * @param serverManager Reference to the server manager.
 * @param requester Reference to the client requesting the nickname change.
 * @param args Vector of arguments, where the first element is the new nickname.
 */
void Nick::execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const
{
    ConfigManager& configManager = serverManager.getConfigManager();

    // Check if no nickname is provided.
    if (args.empty())
    {
        Replier::addToQueue(requester.getFd(), Replier::errNoNicknameGiven, Utils::anyToVec(configManager.getName(),
            requester.getNickname()));
        return;
    }

    const std::string& nickname = args[0];

    // Validate the provided nickname.
    if (!ClientTranslator::nicknameValid(nickname))
    {
        Replier::addToQueue(requester.getFd(), Replier::errOneusNickname, Utils::anyToVec(configManager.getName(),
            requester.getNickname(), nickname));
        return;
    }

    // Check if the nickname is already in use.
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

    // Set the new nickname for the requester.
    requester.setNickname(args[0]);
}
