#include "manager/ConnectionManager.h"
#include "data/Channel.h"
#include <algorithm>
#include <core/Log.h>
#include "unistd.h"
#include <fcntl.h>

ConnectionManager::ConnectionManager() : serverRunning(false)
{

}

ConnectionManager::~ConnectionManager()
{

}

std::vector<pollfd> ConnectionManager::getPollFds() const
{
	return this->pollFds;
}

const int ConnectionManager::initSocket(const int port)
{
	if (port < REGISTERED_PORT_MIN || port > REGISTERED_PORT_MAX)
	{
		throw std::runtime_error(ERROR WRONG_PORT_RANGE);
	}

	const int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0)
	{
		throw std::runtime_error(ERROR SOCKET_FAIL);
	}

	// Set the socket option to allow the socket to be reusable.
	// This is useful for servers that need to restart quickly and bind to the same port
	// without waiting for the operating system to release the port.
	const int option = 1;
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)))
	{
		throw std::runtime_error(ERROR SETSOCKOPT_FAIL);
	}

	// Make the socket non-blocking.
	if (fcntl(serverSocket, F_SETFL, O_NONBLOCK))
	{
		throw std::runtime_error(ERROR F_SETFL_FAIL);
	}

	sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(serverSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
	{
		close(serverSocket);
		throw std::runtime_error(ERROR BIND_FAIL);
	}

	if (listen(serverSocket, SOMAXCONN) < 0)
	{
		close(serverSocket);
		throw std::runtime_error(ERROR LISTEN_FAIL);
	}

	return serverSocket;
}

void ConnectionManager::addServerToPoll(const int fd)
{
	const pollfd listenFd = { fd, POLLIN, 0 };
	this->pollFds.push_back(listenFd);
}

void ConnectionManager::acceptNewClientConnection(const int fd)
{
	Log::msgServer(INFO, NEW_CLIENT_START);

	sockaddr_in addr = {};
	socklen_t addrLen = sizeof(addr);

	// Create fd for the new client and accept it.
	const int clientFd = accept(fd, reinterpret_cast<sockaddr*>(&addr), &addrLen);
	if (clientFd < 0)
	{
		throw std::runtime_error(ERROR ACCEPT_FAIL);
	}

	// Create pollfd for the new client and add it to the pollfd list.
	const pollfd pollFd = { clientFd, POLLIN | POLLOUT, 0 };
	this->pollFdsToAdd.push_back(pollFd);

	Log::msgServer(INFO, "CLIENT", clientFd, NEW_CLIENT_SUCCESS);
}

void ConnectionManager::addNewClientsToPoll()
{
	for (std::vector<pollfd>::iterator it = this->pollFdsToAdd.begin(); it != this->pollFdsToAdd.end(); ++it)
	{
		this->pollFds.push_back(*it);
	}

	this->pollFdsToAdd.clear();
}

void ConnectionManager::queueClientToDeleteFromPoll(const pollfd pollFd)
{
	this->pollFdsToDelete.push_back(pollFd);
}

void ConnectionManager::deleteQueuedClientsFromPoll()
{
	for (std::vector<pollfd>::iterator it = this->pollFdsToDelete.begin(); it != this->pollFdsToDelete.end();)
	{
		if (std::find(this->pollFds.begin(), this->pollFds.end(), *it) != this->pollFds.end())
		{
			it = this->pollFds.erase(it);
		}
		else
		{
			++it;
		}
	}

	this->pollFdsToDelete.clear();
}

//void ConnectionManager::disconnectClient(const int clientFd)
//{
	//this->clients.erase(clientFd);
	//close(clientFd);
	//Log::msgServer(INFO, "CLIENT", clientFd, CLIENT_DISCONNECTED);
//}

void ConnectionManager::handleNicknameCollision(const int newClientFd, const std::string& newClientNickname)
{
	for (std::map<int, Client>::iterator it = this->clients.begin(); it != this->clients.end(); ++it)
	{
		if (newClientNickname == it->second.getNickname() && it->second.getFd() != newClientFd)
		{
			Replier::addToQueue(newClientFd, Replier::errNickCollision, Utils::anyToVec(this->name,
				newClientNickname, it->second.getUsername(), it->second.getHostname()));
			this->disconnectClient(it->second.getFd());
			return;
		}
	}
}