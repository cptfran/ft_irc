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

/**
 * @brief Executes the Ping command.
 * 
 * This method sends a PONG reply to the client that sent the PING command.
 * If no arguments are provided, it replies with the server's name.
 * If an argument is provided, it replies with the server's name and the provided argument.
 * 
 * @param serverManager Reference to the server manager.
 * @param requester Reference to the client that sent the PING command.
 * @param args Vector of arguments provided with the PING command.
 */
void Ping::execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const
{
    ConfigManager& configManager = serverManager.getConfigManager();

    if (args.empty())
    {
        Replier::addToQueue(requester.getFd(), Replier::rplPong, Utils::anyToVec(configManager.getName()));
        return;
    }

    const std::string& serverToReply = args[0];
    Replier::addToQueue(requester.getFd(), Replier::rplPong, Utils::anyToVec(configManager.getName(), serverToReply));
}
