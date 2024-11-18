#include "../include/Server.h"
#include <cerrno>
#include <ClientTranslator.h>
#include <cstdio>
#include "../include/Log.h"
#include <stdexcept>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <iostream>
#include <list>
#include <map>
#include <algorithm>
#include <poll.h>
#include <sstream>
#include "Utils.h"
#include <ctime>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>

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
: fd(-1), name(name), version(version), password(password), availableUserModes("-"),
availableChannelModes("-")
{
	this->commandList.push_back("CAP");
	this->commandList.push_back("JOIN");
	this->commandList.push_back("PASS");
	this->commandList.push_back("NICK");
	this->commandList.push_back("USER");
	this->commandList.push_back("PING");

	const std::time_t now = std::time(NULL);
	creationDate = std::ctime(&now);

	init(port);
}

/**
 * @brief Destructor for the Server class.
 *
 * This destructor is responsible for cleaning up resources when a Server object is destroyed.
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
}

void Server::setVersion(const std::string& version)
{
	this->version = version;
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
void Server::init(const int port)
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
 * adds the client's socket to the list of client sockets, and calls the handleClientPrompt function to manage the client.
 * The function logs messages at various stages to provide information about the server's operation and any errors encountered.
 */
void Server::run()
{
	Log::msgServer(INFO, SERVER_RUN);

	// Add the server to the poll.
	const pollfd listenFd = {this->fd, POLLIN, 0};
    this->pollFds.push_back(listenFd);

	// Main loop, handling new client connections and already running clients.
    while (true)
    {
    	if (poll(this->pollFds.begin().base(), this->pollFds.size(), -1) < 0)
    	{
    		Log::msgServer(ERROR, "Poll error");
    		// continue;
    	}
    	for (std::vector<pollfd>::iterator it = this->pollFds.begin(); it != this->pollFds.end(); ++it)
        {
    		if (it->revents == 0)
        	{
        		continue;
        	}
            if ((it->revents & POLLHUP) == POLLHUP)
        	{
        		break;
        	}
            if ((it->revents & POLLIN) == POLLIN)
            {
    			if (it->fd == this->fd)
                {
                	connectClient();
                	break;
                }
            	handleClientPrompt(this->clients.at(it->fd));
            }
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
	const pollfd pollFd = {clientFd, POLLIN, 0};
	this->pollFds.push_back(pollFd);

	// Create new client object and add it to the clients list.
	const Client client(clientFd);
	this->clients.insert(std::make_pair(clientFd, client));

	Log::msgServer(INFO, "CLIENT", clientFd, NEW_CLIENT_SUCCESS);
}

// TODO: implement certain amount of time client has to send authentication data. (not sure if it's necessary though)
void Server::handleClientPrompt(Client& client) const
{
	char buffer[INPUT_BUFFER_SIZE] = {};
	const ssize_t bytesRead = recv(client.getFd(), buffer, sizeof(buffer) - 1, 0);
	DEBUG_LOG(std::string("CLIENT: \"") + buffer + "\"");
	if (bytesRead <= 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			std::cout << "EAGAIN || EWOULDBLOCK" << std::endl;
			return;
		}
		Log::msgServer(ERROR, "CLIENT", client.getFd(), HANDLE_CLIENT_FAIL);
		close(client.getFd());
		return;
	}
	buffer[bytesRead] = '\0';
	try
	{
		handleCommands(client, buffer);
	}
	catch (const std::exception& e)
	{
		Log::msgServer(INFO, "CLIENT", client.getFd(), e.what());
		Log::msgServer(INFO, "CLIENT", client.getFd(), CLIENT_DISCONNECTED);
		close(client.getFd());
	}
}

void Server::handleCommands(Client& client, const std::string& buffer) const
{
	ClientTranslator translator(buffer);
	translator.fetchCommands(this->commandList);

	std::map<std::string, std::vector<std::string> > commands = translator.getCommands();
	for (std::map<std::string, std::vector<std::string> >::iterator it = commands.begin(); it != commands.end(); ++it)
	{
		if (!isCommandValid(it->first))
		{
			reply(client, errUnknownCommand, Utils::anyToVec(it->first));
			continue;
		}
		if (!isRegistrationCommand(it->first) && !client.registered(this->password))
		{
			reply(client, errNotRegistered, Utils::anyToVec(std::string("")));
			continue;
		}
		executeCommand(client, it->first, it->second);
	}

	if (client.registered(this->password) && !client.getWelcomeRepliesSent())
	{
		Log::msgServer(INFO, "CLIENT", client.getFd(), CLIENT_REGISTER_SUCCESS);

		reply(client, rplWelcome, Utils::anyToVec(this->name, client.getNickname(), client.getUsername()));
		reply(client, rplYourHost, Utils::anyToVec(this->name, client.getNickname(), this->version));
		reply(client, rplCreated, Utils::anyToVec(this->name, client.getNickname(), this->creationDate));
		reply(client, rplMyInfo, Utils::anyToVec(this->name, this->version, this->availableUserModes,
			this->availableChannelModes));

		client.setWelcomeRepliesSent(true);
	}
}

bool Server::isCommandValid(const std::string& command) const
{
	if (find(this->commandList.begin(), this->commandList.end(), command) != this->commandList.end())
	{
		return true;
	}
	return false;
}

bool Server::isRegistrationCommand(const std::string& command) const
{
	if (command == "CAP" || command == "NICK" || command == "USER" || command == "PASS")
	{
		return true;
	}
	return false;
}

void Server::executeCommand(Client& client, const std::string& command, const std::vector<std::string>& args) const
{
	// TODO: check if there are specific error responses if there is something wrong with any of these commands.
	if (command == "CAP" && !args.empty() && args[0] == "LS")
	{
		// TODO: in the end check if it should send any capabilities
		reply(client, rplCap, Utils::anyToVec(std::string("")));
	}
	else if (command == "PASS")
	{
		if (args[0] != this->password)
		{
			reply(client, errPasswdMismatch, Utils::anyToVec(this->name));
			throw std::invalid_argument(WRONG_PASSWORD);
		}
		client.setPassword(args[0]);
	}
	else if (command == "NICK")
	{
		client.setNickname(args[0]);
	}
	else if (command == "USER")
	{
		// TODO: if less than 5 tokens, not enough information, return something to client.

		client.setUsername(args[0]);

		// TODO: handle realname.
		// It must be noted that realname parameter must be the last parameter,
		// because it may contain space characters and must be prefixed with a
		// colon (':') to make sure this is recognised as such.
	}
	else if (command == "JOIN")
	{
		// TODO: handle this command properly. This is just a test code.
		reply(client, rplNoTopic, Utils::anyToVec(args[0]));
	}
	else if (command == "PING")
	{
		if (args.empty())
		{
			reply(client, rplPong, Utils::anyToVec(this->name));
			return;
		}
		reply(client, rplPong, Utils::anyToVec(this->name, args[0]));
	}
}

void Server::reply(const Client& client, const ReplyFunction func, const std::vector<std::string>& args) const
{
	const std::string reply = func(args);
	// DEBUG_LOG(reply);
	send(client.getFd(), reply.c_str(), reply.size(), 0);
}

std::string Server::rplWelcome(const std::vector<std::string>& args)
{
	if (args.size() < 3)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplWelcome()"));
	}
	return ":@" + args[0] + " 001 " + args[1] + " :Welcome to the Internet Relay Network " + args[1] + "!" + args[2]
	+ "@" + args[0] + "\r\n";
}

std::string Server::rplYourHost(const std::vector<std::string>& args)
{
	if (args.size() < 3)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplYourHost()"));
	}
	return ":@" + args[0] + " 002 " + args[1] + " :Your host is " + args[0] + ", running version " + args[2] + "\r\n";
}

std::string Server::rplCreated(const std::vector<std::string>& args)
{
	if (args.size() < 3)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplCreated()"));
	}
	return ":@" + args[0] + " 003 " + args[1] + ": This server was created " + args[2] + "\r\n";
}

std::string Server::rplMyInfo(const std::vector<std::string>& args)
{
	if (args.size() < 4)
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplMyInfo()"));
	}
	return ":@" + args[0] + " 004 " + args[0] + " " + args[1] + " " + args[2] + " " + args[3] + "\r\n";
}

std::string Server::rplPong(const std::vector<std::string>& args)
{
	if (args.empty())
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplPong()"));
	}
	if (args.size() < 2)
	{
		return "PONG :" + args[0] + "\r\n";
	}
	return "Pong :" + args[0] + " " + args[1] + "\r\n";
}

std::string Server::rplCap(const std::vector<std::string>& args)
{
	(void)args;
	return ":server CAP * LS :\r\n";
}


std::string Server::rplNoTopic(const std::vector<std::string>& args)
{
	if (args.empty())
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("rplNoTopic()"));
	}
	return "331 " + args[0] + " :No topic is set\r\n";
}


std::string Server::errUnknownCommand(const std::vector<std::string>& args)
{
	if (args.empty())
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errUnknownCommand()"));
	}
	return "421 " + args[0] + " :Unknown command\r\n";
}

std::string Server::errNotRegistered(const std::vector<std::string>& args)
{
	if (args.empty())
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errNotRegistered()"));
	}
	return "451 " + args[0] + " :You have not registered\r\n";
}

std::string Server::errPasswdMismatch(const std::vector<std::string>& args)
{
	if (args.empty())
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errPasswdMismatch()"));
	}
	return "464 " + args[0] + " :Password incorrect.\r\n";
}

std::string Server::errChannelIsFull(const std::vector<std::string>& args)
{
	if (args.empty())
	{
		throw std::invalid_argument(ERROR + RPL_WRONG_NUM_OF_ARGS("errChannelIsFull()"));
	}
	return "471 " + args[0] + " :Cannot join channel (+1)\r\n";
}