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
			// TODO: probably will have to queue client deletions so I'm sure I'm sending the errors.
			this->clientManager.deleteClient(it->second.getFd());
			
			// Delete user from pollFds.
			const std::vector<pollfd>& pollFds = this->connectionManager.getPollFds();
			for (std::vector<pollfd>::const_iterator pIt = pollFds.begin(); pIt != pollFds.end(); ++pIt)
			{
				if (pIt->fd == it->second.getFd())
				{
					this->connectionManager.queueClientToDeleteFromPoll(*pIt);
					break;
				}
			}

			return;
		}
	}
}