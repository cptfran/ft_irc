#include "core/Server.h"
#include <cerrno>
#include <communication/ClientTranslator.h>
#include <cstdio>
#include "core/Log.h"
#include <stdexcept>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <iostream>
#include <list>
#include <map>
#include <algorithm>
#include <csignal>
#include <poll.h>
#include <sstream>
#include "utils/Utils.h"
#include <ctime>
#include <arpa/inet.h>
#include "communication/Replier.h"
#include "manager/Manager.h"

Server* Server::instance = NULL;

/**
 * @brief Construct a new Server object.
 * 
 * @param name The name of the server.
 * @param version The version of the server.
 * @param password The password for the server.
 * @param port The port number on which the server will listen.
 */
Server::Server(const std::string& name, const std::string& version, const std::string& password, const int port)
	: running(true), channelManager(), clientManager(), commandManager(), configManager(name, version, password),
	connectionManager()
{
	this->configManager.setFd(this->connectionManager.initSocket(port));
	instance = this;
	signal(SIGINT, signalHandler);
}

Server::~Server()
{
	instance = NULL;
}

/**
 * @brief Run the server, handling new client connections and already running clients.
 */
void Server::run()
{
	Log::msgServer(Log::INFO, Log::SERVER_RUN);

	// Add the server to the poll.
	const pollfd listenFd = {this->configManager.getFd(), POLLIN, 0};
	this->pollFds.push_back(listenFd);

	// Main loop, handling new client connections and already running clients.
	while (running)
	{
		const int pollResult = poll(&this->pollFds[0], this->pollFds.size(), 1000);
		if (pollResult < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			Log::msgServer(Log::ERROR, "Poll error " + std::string(strerror(errno)));
			return;
		}

		this->connectionManager.addNewClientsToPoll(this->pollFds);
		this->connectionManager.deleteQueuedClientsFromPoll(this->pollFds);
		this->timeoutHandler();
		this->eventHandler();
	}
}

/**
 * @brief Stop the server.
 */
void Server::stop()
{
	this->running = false;
}

/**
 * @brief Signal handler for SIGINT.
 * 
 * @param signum The signal number.
 */
void Server::signalHandler(const int signum)
{
	if (signum == SIGINT && instance)
	{
		instance->stop();
	}
}

/**
 * @brief Handle events from the poll.
 */
void Server::eventHandler()
{
	for (std::vector<pollfd>::iterator it = this->pollFds.begin(); it != this->pollFds.end(); ++it)
	{
		if ((it->revents & POLLHUP) == POLLHUP)
		{
			this->clientRemovalHandler(it->fd);
		}
		else if ((it->revents & POLLOUT) == POLLOUT && Replier::clientInQueue(it->fd))
		{
			Replier::sendFromQueue(it->fd);
		}
		else if ((it->revents & POLLIN) == POLLIN)
		{
			if (it->fd == this->configManager.getFd())
			{
				clientJoinHandler();
			}
			else
			{
				clientRequestHandler(it->fd);
			}
		}
	}
}

/**
 * @brief Handle client timeouts.
 */
void Server::timeoutHandler()
{
	std::map<int, Client> clients = this->clientManager.getClients();

	for (std::vector<pollfd>::iterator it = this->pollFds.begin(); it != this->pollFds.end(); ++it)
	{
		std::map<int, Client>::const_iterator clientIt = clients.find(it->fd);
		if (clientIt != clients.end() &&
			!clientIt->second.registered(this->configManager.getPassword()) &&
			difftime(std::time(0), clientIt->second.getTimeConnected()) > ConfigManager::TIME_FOR_CLIENT_TO_REGISTER)
		{
			Replier::addToQueue(it->fd, Replier::errClosingLink, Utils::anyToVec(clientIt->second.getNickname(),
				clientIt->second.getHostname()));
			this->connectionManager.queueClientToDeleteFromPoll(it->fd);
			this->clientManager.deleteClient(it->fd);
		}
	}
}

/**
 * @brief Handle a new client joining.
 */
void Server::clientJoinHandler()
{
	pollfd newPollFd = this->connectionManager.acceptNewClientConnection(this->configManager.getFd());
	this->connectionManager.queueNewClientToAddToPoll(newPollFd);
	this->clientManager.addClient(newPollFd.fd);
}

/**
 * @brief Handle a client request.
 * 
 * @param fd The file descriptor of the client.
 */
void Server::clientRequestHandler(const int fd)
{
	try
	{
		this->clientManager.getClientByFd(fd).addToBuffer(ClientTranslator::parseClientBufferFromRecv(fd));
		Manager manager(this->channelManager, this->clientManager, this->commandManager, this->configManager, 
			this->connectionManager);
		commandManager.executeCommands(manager, this->clientManager.getClientByFd(fd));
	}
	catch (std::exception&)
	{
		this->clientRemovalHandler(fd);
	}
}

/**
 * @brief Handle a client removal.
 * 
 * @param fd The file descriptor of the client.
 */
void Server::clientRemovalHandler(const int fd)
{
	this->channelManager.deleteClientFromChannels(this->clientManager.getClientByFd(fd));
	this->clientManager.deleteClient(fd);
	this->connectionManager.queueClientToDeleteFromPoll(fd);
}