#include "manager/ClientManager.h"
#include <vector>

ClientManager::ClientManager()
{

}

ClientManager::~ClientManager()
{

}

void ClientManager::addClient(const int clientFd, const Client& client)
{
	// TODO: need to check what exactly should be here.
}

void ClientManager::removeClient(const int clientFd)
{
	// TODO: need to check what exactly should be here.
}

Client* ClientManager::findClientByNickname(const std::string& nicknameToFind)
{
	for (std::map<int, Client>::iterator it = this->clients.begin(); it != this->clients.end(); ++it)
	{
		if (it->second.getNickname() == nicknameToFind)
		{
			return &it->second;
		}
	}
	return NULL;
}

std::map<int, Client> ClientManager::getClients() const
{
	return this->clients;
}

std::string ClientManager::getClientHostname(sockaddr_in& addr, socklen_t addrLen, const int clientFd) const
{
	// A buffer to hold the client's IP address in human-readable form.
	char client_ip[INET_ADDRSTRLEN];

	// Fills the addr structure with the address information of the client connected to clientFd.
	if (getpeername(clientFd, reinterpret_cast<sockaddr*>(&addr), &addrLen) == -1)
	{
		return "unknown";
	}

	// Converts the binary IP address in addr.sin_addr to a human-readable string and stores it in client_ip.
	inet_ntop(AF_INET, &addr.sin_addr, client_ip, sizeof(client_ip));

	// Resolves the IP address in addr.sin_addr to a hostname.
	const hostent* host_entry = gethostbyaddr(&addr.sin_addr, sizeof(addr.sin_addr), AF_INET);

	// Hostname resolution failed, and the function returns the IP address as a string.
	if (host_entry == NULL)
	{
		return std::string(client_ip);
	}

	// Function returns the resolved hostname as a std::string.
	return std::string(host_entry->h_name);
}