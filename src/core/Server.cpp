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
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "communication/Replier.h"

Server* Server::instance = NULL;

Server::Server(const std::string& name, const std::string& version, const std::string& password, const int port)
	: config(name, version, password), channelManager(), clientManager(), commandManager(), connectionManager()
{
	this->config.setFd(this->connectionManager.initSocket(port));
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
	this->this->connectionManager.addServerToPoll();

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

		this->connectionManager.handleTimeouts();
		this->connectionManager.addNewClientsToPoll();
		this->connectionManager.deleteQueuedClientsFromPoll();

		for (std::vector<pollfd>::iterator it = this->connectionManager.getPollFds().begin(); 
			it != this->connectionManager.getPollFds().end(); ++it)
		{
			Client& currClient = this->clientManager.getClients().at(it->fd);

			if ((it->revents & POLLHUP) == POLLHUP)
			{
				this->channelManager.deleteClientFromChannels(currClient);
				this->clientManager.deleteClient(it->fd);
				this->connectionManager.queueClientToDeleteFromPoll(*it);
			}
			else if ((it->revents & POLLOUT) == POLLOUT && Replier::clientInQueue(it->fd))
			{
				Replier::sendFromQueue(it->fd);
			}
			else if ((it->revents & POLLIN) == POLLIN)
			{
				if (it->fd == this->config.getFd())
				{
					this->connectionManager.acceptNewClientConnection(it->fd);
					this->clientManager.addClient(it->fd);
				}
				else
				{
					try
					{
						currClient.addToBuffer(ClientTranslator::parseClientBufferFromRecv(it->fd));
						commandManager.executeCommands(currClient, this->config.getPassword());
					}
					catch (std::exception&)
					{
						this->channelManager.deleteClientFromChannels(currClient);
						this->clientManager.deleteClient(it->fd);
						this->connectionManager.queueClientToDeleteFromPoll(*it);
					}
				}
			}
			++it;
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
			!clientIt->second.registered(this->config.getPassword()) &&
			difftime(std::time(0), clientIt->second.getTimeConnected()) > TIME_FOR_CLIENT_TO_REGISTER)
		{
			Replier::addToQueue(it->fd, Replier::errClosingLink, Utils::anyToVec(clientIt->second.getNickname(),
				clientIt->second.getHostname()));
			this->connectionManager.queueClientToDeleteFromPoll(*it);
			this->clientManager.deleteClient(it->fd);
		}
	}
}