#include "commands/Who.h"
#include "communication/ClientTranslator.h"
#include "communication/Replier.h"
#include "utils/Utils.h"

Who::Who()
{

}

Who::~Who()
{

}

/**
 * @brief Executes the WHO command.
 *
 * This method processes the WHO command issued by a client. It determines whether the client
 * is requesting Log::INFOrmation about a channel or about users. If the command is for a channel,
 * it delegates the request to handleChannel(). If the command is for users, it delegates the
 * request to handleUsers().
 *
 * @param serverManager Reference to the server manager.
 * @param requester Reference to the client who requested the command.
 * @param args List of arguments passed to the command.
 */
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

    // User prompting for channel Log::INFO.
    if (mask[0] == '#')
    {
        bool operatorOnly = (args.size() == 2 && args[1] == "o");
        handleChannel(serverManager, mask, requester, operatorOnly);
        return;
    }

    // User prompting for users Log::INFO.
    handleUsers(serverManager, requester, mask);
}

/**
 * @brief Handles the WHO command for a specific channel.
 *
 * This method processes the WHO command when the client requests Log::INFOrmation about a specific
 * channel. It retrieves the list of users in the channel and sends the Log::INFOrmation back to the
 * requester. If the channel does not exist, it sends an Log::ERROR message to the requester.
 *
 * @param serverManager Reference to the server manager.
 * @param mask The channel mask.
 * @param requester Reference to the client who requested the command.
 * @param operatorOnly Flag indicating if only operators should be listed.
 */
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

/**
 * @brief Handles the WHO command for users.
 * 
 * This method processes the WHO command when the client requests Log::INFOrmation about users.
 * It iterates through the list of clients and checks if they match the given mask. If a user
 * matches the mask and is not invisible or shares a common channel with the requester, their
 * Log::INFOrmation is sent back to the requester.
 *
 * @param serverManager Reference to the server manager.
 * @param requester Reference to the client who requested the command.
 * @param mask The user mask.
 */
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

/**
 * @brief Checks if a user matches a given mask.
 *
 * This method checks if a user's hostname, server name, real name, or nickname matches the
 * given mask using wildcard matching.
 *
 * @param serverName The name of the server.
 * @param requester Reference to the client being checked.
 * @param mask The mask to match against.
 * @return True if the user matches the mask, false otherwise.
 */
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