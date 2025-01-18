#include "manager/ConnectionManager.h"
#include "manager/ConfigManager.h"
#include "data/Channel.h"
#include <algorithm>
#include <core/Log.h>
#include "unistd.h"
#include <fcntl.h>
#include <netdb.h>

ConnectionManager::ConnectionManager()
{

}

ConnectionManager::~ConnectionManager()
{

}

/**
 * @brief Initializes a server socket.
 * 
 * This method creates a server socket, sets it to be reusable and non-blocking,
 * binds it to the specified port, and starts listening for incoming connections.
 *
 * @param port The port number to bind the server socket to.
 * @return The file descriptor of the created server socket.
 * @throws std::runtime_error if the port is out of range, or if socket operations fail.
 */
int ConnectionManager::initSocket(const int port)
{
	if (port < ConfigManager::REGISTERED_PORT_MIN || port > ConfigManager::REGISTERED_PORT_MAX)
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

/**
 * @brief Accepts a new client connection.
 * 
 * This method waits for a new client to connect to the server socket specified by serverFd.
 * Once a client connects, it creates a new file descriptor for the client connection and
 * returns a pollfd structure for monitoring the client's socket.
 * 
 * @param serverFd The file descriptor of the server socket.
 * @return A pollfd structure for the new client connection.
 * @throws std::runtime_error if accepting the client connection fails.
 */
pollfd ConnectionManager::acceptNewClientConnection(const int serverFd)
{
	Log::msgServer(INFO, NEW_CLIENT_START);

	sockaddr_in addr = {};
	socklen_t addrLen = sizeof(addr);

	// Create fd for the new client and accept it.
	const int clientFd = accept(serverFd, reinterpret_cast<sockaddr*>(&addr), &addrLen);
	if (clientFd < 0)
	{
		throw std::runtime_error(ERROR ACCEPT_FAIL);
	}

	Log::msgServer(INFO, "CLIENT", clientFd, NEW_CLIENT_SUCCESS);

	// Create pollfd for the new client and add it to the pollfd list.
	pollfd pollFd = { clientFd, POLLIN | POLLOUT, 0 };
	return pollFd;
}

void ConnectionManager::queueNewClientToAddToPoll(const pollfd pollFd)
{
	this->pollFdsToAdd.push_back(pollFd);
}

void ConnectionManager::addNewClientsToPoll(std::vector<pollfd>& pollFds)
{
	for (std::vector<pollfd>::iterator it = this->pollFdsToAdd.begin(); it != this->pollFdsToAdd.end(); ++it)
	{
		pollFds.push_back(*it);
	}

	this->pollFdsToAdd.clear();
}

void ConnectionManager::queueClientToDeleteFromPoll(const int fd)
{
	this->fdsToDelete.push_back(fd);
}

void ConnectionManager::deleteQueuedClientsFromPoll(std::vector<pollfd>& pollFds)
{
	// Iterate over fdsToDelete and remove matching pollfd entries from pollFds.
	for (std::vector<int>::iterator delIt = this->fdsToDelete.begin(); delIt != this->fdsToDelete.end(); ++delIt)
	{
		for (std::vector<pollfd>::iterator it = pollFds.begin(); it != pollFds.end();)
		{
			if (it->fd == *delIt)
			{
				Log::msgServer(INFO, "CLIENT", it->fd, CLIENT_DISCONNECTED);
				it = pollFds.erase(it);
				close(it->fd);
			}
			else
			{
				++it;
			}
		}
	}

	// Clear the list of fdsToDelete after processing.
	this->fdsToDelete.clear();
}
