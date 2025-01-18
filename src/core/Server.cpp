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

void Server::run()
{
	Log::msgServer(INFO, SERVER_RUN);

	// Add the server to the poll.
	this->connectionManager.addServerToPoll(this->configManager.getFd());

	// Main loop, handling new client connections and already running clients.
	while (running)
	{
		const int pollResult = poll(&this->connectionManager.getPollFds()[0], this->connectionManager.getPollFds().size(), 1000);
		if (pollResult < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			Log::msgServer(ERROR, "Poll error");
			return;
		}

		this->handleTimeouts();
		this->connectionManager.addNewClientsToPoll();
		this->connectionManager.deleteQueuedClientsFromPoll();

		const std::vector<pollfd>& pollFds = this->connectionManager.getPollFds();
		for (std::vector<pollfd>::const_iterator it = pollFds.begin(); it != pollFds.end(); ++it)
		{
			if ((it->revents & POLLHUP) == POLLHUP)
			{
				this->channelManager.deleteClientFromChannels(this->clientManager.getClientByFd(it->fd));
				this->clientManager.deleteClient(it->fd);
				this->connectionManager.queueClientToDeleteFromPoll(*it);
			}
			else if ((it->revents & POLLOUT) == POLLOUT && Replier::clientInQueue(it->fd))
			{
				Replier::sendFromQueue(it->fd);
			}
			else if ((it->revents & POLLIN) == POLLIN)
			{
				if (it->fd == this->configManager.getFd())
				{
					this->connectionManager.acceptNewClientConnection(it->fd);
					this->clientManager.addClient(it->fd);
				}
				else
				{
					try
					{
						this->clientManager.getClientByFd(it->fd).addToBuffer(ClientTranslator::parseClientBufferFromRecv(it->fd));
						Manager manager(this->channelManager, this->clientManager,
							this->commandManager, this->configManager, this->connectionManager);
						commandManager.executeCommands(manager, this->clientManager.getClientByFd(it->fd));
					}
					catch (std::exception&)
					{
						this->channelManager.deleteClientFromChannels(this->clientManager.getClientByFd(it->fd));
						this->clientManager.deleteClient(it->fd);
						this->connectionManager.queueClientToDeleteFromPoll(*it);
					}
				}
			}
		}
	}
}

void Server::stop()
{
	this->running = false;
}

void Server::signalHandler(const int signum)
{
	if (signum == SIGINT && instance)
	{
		instance->stop();
	}
}

void Server::handleTimeouts()
{
	std::vector<pollfd> pollFds = this->connectionManager.getPollFds();
	std::map<int, Client> clients = this->clientManager.getClients();

	for (std::vector<pollfd>::iterator it = pollFds.begin(); it != pollFds.end(); ++it)
	{
		std::map<int, Client>::const_iterator clientIt = clients.find(it->fd);
		if (clientIt != clients.end() &&
			!clientIt->second.registered(this->configManager.getPassword()) &&
			difftime(std::time(0), clientIt->second.getTimeConnected()) > TIME_FOR_CLIENT_TO_REGISTER)
		{
			Replier::addToQueue(it->fd, Replier::errClosingLink, Utils::anyToVec(clientIt->second.getNickname(),
				clientIt->second.getHostname()));
			// TODO: check if can send the message first and then delete.
			this->connectionManager.queueClientToDeleteFromPoll(*it);
			this->clientManager.deleteClient(it->fd);
		}
	}
}