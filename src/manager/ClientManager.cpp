#include "manager/ClientManager.h"
#include <vector>
#include <csignal>
#include <unistd.h>
#include <arpa/inet.h>
#include <communication/Replier.h>
#include <utils/Utils.h>

#include "core/Log.h"

ClientManager::ClientManager()
{

}

ClientManager::~ClientManager()
{
	for (std::map<int, Client>::iterator it = this->clients.begin(); it != this->clients.end(); ++it)
	{
		const int clientFd = it->second.getFd();
		if (clientFd != -1)
		{
			close(clientFd);
		}
	}
}

void ClientManager::addClient(const int clientFd)
{
	sockaddr_in addr = {};
	const socklen_t addrLen = sizeof(addr);

	// Create new client object.
	Client client(clientFd);

	// Fetch and set client hostname.
	client.setHostname(this->findClientHostname(addr, addrLen, clientFd));

	// Add client to the list.
	this->clients.insert(std::make_pair(clientFd, client));
}

void ClientManager::deleteClient(const int clientFd)
{
	this->clients.erase(clientFd);
}

Client& ClientManager::getClientByFd(const int fd)
{
	return this->clients.at(fd);
}

Client* ClientManager::getClientByNickname(const std::string& nicknameToFind)
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

const std::map<int, Client>& ClientManager::getClients() const
{
	return this->clients;
}

std::string ClientManager::findClientHostname(sockaddr_in& addr, socklen_t addrLen, const int clientFd) const
{
	// A buffer to hold the client's IP address in human-readable form.
	char client_ip[INET_ADDRSTRLEN];

	// Fills the addr structure with the address Log::INFOrmation of the client connected to clientFd.
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

