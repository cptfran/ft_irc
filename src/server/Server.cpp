#include "server/Server.h"
#include <cerrno>
#include <client/ClientTranslator.h>
#include <cstdio>
#include "server/Log.h"
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
#include "commands/Cap.h"
#include "commands/Join.h"
#include "commands/Nick.h"
#include "commands/Pass.h"
#include "commands/Ping.h"
#include "commands/User.h"
#include "commands/Kick.h"
#include "commands/Topic.h"
#include "commands/Invite.h"
#include "commands/Mode.h"
#include "commands/Part.h"
#include "commands/Who.h"
#include "commands/Privmsg.h"
#include "replier/Replier.h"

Server* Server::instance = NULL;

/**
 * @brief Constructor for the Server class.
 *
 * This constructor initializes a Server object with a specified password.
 * It performs the following steps:
 * 1. Initializes the server's file descriptor (fd) to -1, indicating that no socket is currently open.
 * 2. Sets the server's name to the provided value.
 * 3. Sets the server's version to the provided value.
 * 4. Sets the server's password to the provided value.
 * 5. Sets the server's creation date for the current date.
 * 6. Sets set of reply commands, that will be sent to clients.
 * @param name The name of the server.
 * @param version The current version of the server.
 * @param password The password to be used for client authentication.
 */
Server::Server(const std::string& name, const std::string& version, const std::string& password, const int port)
: running(true), fd(-1), name(name), version(version), password(password), availableUserModes(std::string("i")),
availableChannelModes(std::string("itkol"))
{
	this->validCommands["CAP"] = new Cap();
	this->validCommands["INVITE"] = new Invite();
	this->validCommands["JOIN"] = new Join();
	this->validCommands["KICK"] = new Kick();
	this->validCommands["MODE"] = new Mode();
	this->validCommands["NICK"] = new Nick();
	this->validCommands["PART"] = new Part();
	this->validCommands["PASS"] = new Pass();
	this->validCommands["PING"] = new Ping();
	this->validCommands["PRIVMSG"] = new Privmsg();
	this->validCommands["TOPIC"] = new Topic();
	this->validCommands["USER"] = new User();
	this->validCommands["WHO"] = new Who();

	const std::time_t now = std::time(NULL);
	creationDate = std::ctime(&now);
	if (creationDate[creationDate.size() - 1] == '\n')
	{
		creationDate.resize(creationDate.size() - 1);
	}
	this->initSocket(port);

	instance = this;
	signal(SIGINT, signalHandler);
}

/**
 * @brief Destructor for the Server class.
 *
 * This destructor is responsible for cleaning up resource when a Server object is destroyed.
 * It performs the following steps:
 * 1. Iterates through the list of client sockets and closes each one.
 * 2. Closes the server's main socket if it is open.
 *
 * This ensures that all open sockets are properly closed, preventing resource leaks.
 */
Server::~Server()
{
	if (this->fd != -1)
	{
		close(this->fd);
	}

	for (std::map<int, Client>::iterator it = this->clients.begin(); it != this->clients.end(); ++it)
	{
		const int clientFd = it->second.getFd();
		if (clientFd != -1)
		{
			close(clientFd);
		}
	}

	for (std::map<std::string, Command*>::iterator it = this->validCommands.begin(); it != this->validCommands.end();
		++it)
	{
		delete it->second;
	}

	instance = NULL;
}

void Server::setVersion(const std::string& version)
{
	this->version = version;
}

std::string Server::getName() const
{
	return this->name;
}


std::string Server::getPassword() const
{
	return this->password;
}

std::map<int, Client> Server::getClients() const
{
	return this->clients;
}

Channel* Server::getNewestChannel()
{
    return &this->channels.back();
}

Channel* Server::getChannel(const std::string& channelName)
{
	for (std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); ++it)
	{
		const std::string& currChannelName = it->getName();
		if (currChannelName == channelName)
		{
			return &(*it);
		}
	}
	return NULL;
}

std::string Server::getAvailableChannelModes() const
{
	return this->availableChannelModes;
}

std::string Server::getAvailableUserModes() const
{
	return this->availableUserModes;
}

Client* Server::findClientByNickname(const std::string& nicknameToFind)
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

bool Server::usersHaveCommonChannel(const std::string& nickname1, const std::string& nickname2) const
{
	for (std::vector<Channel>::const_iterator it = this->channels.begin(); it != this->channels.end(); ++it)
	{
		if (it->isUserOnChannel(nickname1) && it->isUserOnChannel(nickname2))
		{
			return true;
		}
	}

	return false;
}

void Server::signalHandler(const int signum)
{
	if (signum == SIGINT && instance)
	{
		instance->stop();
	}
}

/**
 * @brief Initializes the server with the specified port.
 *
 * This function sets up the server to listen for incoming connections on the specified port.
 * It performs the following steps:
 * 1. Validates that the port number is within the registered port range.
 * 2. Creates a socket for the server.
 * 3. Configures the server's address structure with the specified port and binds the socket to it.
 * 4. Sets the socket to listen for incoming connections.
 *
 * If any step fails, the function throws a runtime error with an appropriate error message.
 *
 * @param port The port number on which the server will listen for incoming connections.
 * @throws std::runtime_error If the port is out of range, socket creation fails, binding fails, or listening fails.
 */
void Server::initSocket(const int port)
{
	if (port < REGISTERED_PORT_MIN || port > REGISTERED_PORT_MAX)
	{
		throw std::runtime_error(ERROR WRONG_PORT_RANGE);
	}

	this->fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->fd < 0)
	{
		throw std::runtime_error(ERROR SOCKET_FAIL);
	}

	// Set the socket option to allow the socket to be reusable.
	// This is useful for servers that need to restart quickly and bind to the same port
	// without waiting for the operating system to release the port.
	const int option = 1;
	if (setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)))
	{
		throw std::runtime_error(ERROR SETSOCKOPT_FAIL);
	}

	// Make the socket non-blocking.
	if (fcntl(this->fd, F_SETFL, O_NONBLOCK))
	{
		throw std::runtime_error(ERROR F_SETFL_FAIL);
	}

	sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(this->fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
	{
		close(this->fd);
		throw std::runtime_error(ERROR BIND_FAIL);
	}

	if (listen(this->fd, SOMAXCONN) < 0)
	{
		close(this->fd);
		throw std::runtime_error(ERROR LISTEN_FAIL);
	}
}

/**
 * @brief Runs the server, accepting and handling client connections.
 *
 * This function enters an infinite loop to continuously accept new client connections.
 * For each new connection, it attempts to authenticate the client using the authenticateClient function.
 * If the authentication fails, it closes the connection and logs an error message.
 * If the authentication succeeds, it logs a success message, sends a welcome message to the client,
 * adds the client's socket to the list of client sockets, and calls the handleRecv function to manage the client.
 * The function logs messages at various stages to provide information about the server's operation and any errors encountered.
 */
void Server::run()
{
	Log::msgServer(INFO, SERVER_RUN);

	// Add the server to the poll.
	const pollfd listenFd = {this->fd, POLLIN, 0};
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
    		Log::msgServer(ERROR, "Poll error");
    		return;
    	}

    	handleTimeouts();
		addNewClientsToPoll();

    	for (std::vector<pollfd>::iterator it = this->pollFds.begin(); it != this->pollFds.end();)
        {
            if ((it->revents & POLLHUP) == POLLHUP)
        	{
				disconnectClient(it->fd);
            	it = this->pollFds.erase(it);
				continue;
        	}
			else if ((it->revents & POLLOUT) == POLLOUT && Replier::clientInQueue(it->fd))
			{
				Replier::sendFromQueue(it->fd);
			}
            else if ((it->revents & POLLIN) == POLLIN)
            {
    			if (it->fd == this->fd)
                {

                	this->connectClient();
                }
				else
				{
					this->handleRecv(this->clients.at(it->fd));
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

void Server::handleNicknameCollision(const int newClientFd, const std::string& newClientNickname)
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

void Server::addChannel(const Channel& channel)
{
	channels.push_back(channel);
}

void Server::deleteChannelIfEmpty(const Channel& channel)
{
	if (channel.getNumOfJoinedUsers() == 0)
	{
		const std::vector<Channel>::iterator it = std::find(this->channels.begin(), this->channels.end(),
			channel);
		if (it != this->channels.end())
		{
			this->channels.erase(it);
		}
	}
}

void Server::connectClient()
{
	Log::msgServer(INFO, NEW_CLIENT_START);

	sockaddr_in addr = {};
	socklen_t addrLen = sizeof(addr);

	// Create fd for the new client and accept it.
	const int clientFd = accept(this->fd, reinterpret_cast<sockaddr*>(&addr), &addrLen);
	if (clientFd < 0)
	{
		throw std::runtime_error(ERROR ACCEPT_FAIL);
	}

	// Create pollfd for the new client and add it to the pollfd list.
	const pollfd pollFd = {clientFd, POLLIN | POLLOUT, 0};
	this->pollFdsToAdd.push_back(pollFd);

	// Create new client object.
	Client client(clientFd);

	// Fetch client hostname.
	client.setHostname(getClientHostname(addr, addrLen, clientFd));

	// Add new client to the clients list.
	this->clients.insert(std::make_pair(clientFd, client));

	Log::msgServer(INFO, "CLIENT", clientFd, NEW_CLIENT_SUCCESS);
}

std::string Server::getClientHostname(sockaddr_in& addr, socklen_t addrLen, const int clientFd) const
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

void Server::disconnectClient(const int clientFd)
{
	for (std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); ++it)
	{
		it->ejectUser(*this, this->clients.at(clientFd).getNickname());
	}
	this->clients.erase(clientFd);
	close(clientFd);
	Log::msgServer(INFO, "CLIENT", clientFd, CLIENT_DISCONNECTED);
}

void Server::handleRecv(Client& client)
{
	char recvBuffer[INPUT_BUFFER_SIZE] = {};
	const ssize_t bytesRead = recv(client.getFd(), recvBuffer, sizeof(recvBuffer) - 1, 0);
	DEBUG_LOG(std::string("CLIENT[" + Utils::intToString(client.getFd()) + "]: \"") + recvBuffer + "\"");
	if (bytesRead <= 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			std::cout << "EAGAIN || EWOULDBLOCK" << std::endl;
			return;
		}
		this->disconnectClient(client.getFd());
		return;
	}
	recvBuffer[bytesRead] = '\0';
	client.addToBuffer(recvBuffer);
	
	while (true)
	{
		std::string msg = client.departCompleteMsgFromBuffer();
		if (msg.empty())
		{
			return;
		}
		try
		{
			this->executeCommand(client, msg);
		}
		catch (const std::exception& e)
		{
			Log::msgServer(INFO, "CLIENT", client.getFd(), e.what());
			this->disconnectClient(client.getFd());
		}
	}
}

void Server::executeCommand(Client& client, const std::string& buffer)
{
	std::pair<std::string, std::vector<std::string> > cmdWithArgs = ClientTranslator::fetchCmdAndArgs(buffer);
	if (this->validCommands.find(cmdWithArgs.first) == this->validCommands.end())
	{
		Replier::addToQueue(client.getFd(), Replier::errUnknownCommand, Utils::anyToVec(this->name,
			client.getNickname(), cmdWithArgs.first));
		return;
	}
	if (cmdWithArgs.first == "PRIVMSG")
	{
		std::cout << "asgfasg" << std::endl;
	}

	this->validCommands.at(cmdWithArgs.first)->execute(*this, client, cmdWithArgs.second);

	if (client.registered(this->password) && !client.getWelcomeRepliesSent())
	{
		Log::msgServer(INFO, "CLIENT", client.getFd(), CLIENT_REGISTER_SUCCESS);

		Replier::addToQueue(client.getFd(), Replier::rplWelcome, Utils::anyToVec(this->name, client.getNickname(),
			client.getUsername(), client.getHostname()));
		Replier::addToQueue(client.getFd(), Replier::rplYourHost, Utils::anyToVec(this->name, client.getNickname(),
			this->version));
		Replier::addToQueue(client.getFd(), Replier::rplCreated, Utils::anyToVec(this->name, client.getNickname(),
			this->creationDate));
		Replier::addToQueue(client.getFd(), Replier::rplMyInfo, Utils::anyToVec(this->name, client.getNickname(),
			this->version, this->availableUserModes, this->availableChannelModes));

		client.setWelcomeRepliesSent(true);
	}
}

void Server::handleTimeouts()
{
	for (std::vector<pollfd>::iterator it = this->pollFds.begin(); it != this->pollFds.end();)
	{
		std::map<int, Client>::iterator clientIt = this->clients.find(it->fd);
		if (clientIt != this->clients.end() &&
			!clientIt->second.registered(this->password) &&
			difftime(std::time(0), clientIt->second.getTimeConnected()) > TIME_FOR_CLIENT_TO_REGISTER)
		{
			Replier::addToQueue(it->fd, Replier::errClosingLink, Utils::anyToVec(clientIt->second.getNickname(),
				clientIt->second.getHostname()));
			disconnectClient(it->fd);
			it = this->pollFds.erase(it);
			continue;
		}
		++it;
	}
}

void Server::addNewClientsToPoll()
{
	for (std::vector<pollfd>::iterator it = this->pollFdsToAdd.begin(); it != this->pollFdsToAdd.end(); ++it)
	{
		this->pollFds.push_back(*it);
	}

	this->pollFdsToAdd.clear();
}
