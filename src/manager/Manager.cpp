#include "communication/Replier.h"
#include "manager/Manager.h"
#include <utils/Utils.h>

Manager::Manager(ChannelManager& channelManager, ClientManager& clientManager, CommandManager& commandManager,
    ConfigManager& configManager, ConnectionManager& connectionManager) :
    channelManager(channelManager), clientManager(clientManager), commandManager(commandManager), 
    configManager(configManager), connectionManager(connectionManager) 
{

}

Manager::~Manager()
{

}

ChannelManager& Manager::getChannelManager() const
{
    return this->channelManager;
}

ClientManager& Manager::getClientManager() const
{
    return this->clientManager; 
}

CommandManager& Manager::getCommandManager() const
{
    return this->commandManager; 
}

ConfigManager& Manager::getConfigManager() const
{
    return this->configManager;
}

ConnectionManager& Manager::getConnectionManager() const
{
    return this->connectionManager;
}

/**
 * @brief Handles a case when new client connects and nickname collision occurs. 
 * 
 * Handles nickname collision by checking if the new client's nickname
 * already exists in the client list. If a collision is found, the existing
 * client with the same nickname is removed from the client list and pollFds.
 *
 * @param newClientFd The file descriptor of the new client.
 * @param newClientNickname The nickname of the new client.
 */
void Manager::handleNicknameCollision(const int newClientFd, const std::string& newClientNickname)
{
    // Get client list, iterate and find collision.
    const std::map<int, Client>& clients = this->clientManager.getClients();
    for (std::map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it)
    {
        // Collision found, delete user from client list.
        if (newClientNickname == it->second.getNickname() && it->second.getFd() != newClientFd)
        {
            Replier::addToQueue(newClientFd, Replier::errNickCollision, Utils::anyToVec(this->configManager.getName(),
                newClientNickname, it->second.getUsername(), it->second.getHostname()));
	        
	        this->connectionManager.queueClientToDeleteFromPoll(it->second.getFd());
            this->clientManager.deleteClient(it->second.getFd());
            return;
        }
    }
}