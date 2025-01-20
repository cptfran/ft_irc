#include "commands/Pass.h"
#include "core/Server.h"
#include "utils/Utils.h"
#include "core/Log.h"
#include "communication/Replier.h"

Pass::Pass() : Command()
{

}

Pass::~Pass()
{

}

/**
 * @brief Executes the PASS command to authenticate a client with a password.
 * 
 * @param serverManager Reference to the server's Manager object.
 * @param requester Reference to the Client object making the request.
 * @param args Vector of arguments passed with the command.
 */
void Pass::execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const
{
    const ConfigManager& configManager = serverManager.getConfigManager();

    // Check if no password was provided.
    if (args.empty())
    {
        Replier::addToQueue(requester.getFd(), Replier::errPasswdMismatch, Utils::anyToVec(configManager.getName(),
            requester.getNickname()));
        return;
    }

    const std::string& enteredPassword = args[0];
    // Check if the provided password does not match the server's password.
    if (!configManager.passwordValidator(enteredPassword))
    {
        Replier::addToQueue(requester.getFd(), Replier::errPasswdMismatch, Utils::anyToVec(configManager.getName(),
            requester.getNickname()));
        return;
    }

    // Set the client's password.
    requester.setPasswordAccepted(true);
}
