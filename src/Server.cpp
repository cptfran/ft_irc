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
#include <bits/stl_algo.h>
#include <poll.h>
#include <sstream>
#include "Utils.h"
#include <ctime>
#include <fcntl.h>
#include <asm-generic/errno.h>
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
Server::Server(const std::string& name, const std::string& version, const std::string& password)
: fd(-1), address(), name(name), version(version), password(password), availableUserModes("-"),
availableChannelModes("-")
{
	const std::time_t now = std::time(NULL);
	creationDate = std::ctime(&now);
	this->replies[001] = rplWelcome;
	this->replies[002] = rplYourHost;
	this->replies[003] = rplCreated;
	this->replies[004] = rplMyInfo;
	this->replies[332] = rplNoTopic;
	this->replies[421] = errUnknownCommand;
	this->replies[471] = errChannelIsFull;
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
	if (this->fd == -1)
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

	this->address.sin_family = AF_INET;
	this->address.sin_port = htons(port);
	this->address.sin_addr.s_addr = INADDR_ANY;
	if (bind(this->fd, reinterpret_cast<sockaddr*>(&this->address), sizeof(this->address)) < 0)
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
// TODO: check line by line the difference between here and github code.
void Server::run()
{
	Log::msgServer(INFO, SERVER_RUN);

	// Add the server to the poll.
	const pollfd listen_fd = {this->fd, POLLIN, 0};
    this->pollFds.push_back(listen_fd);

	// Main loop, handling new client connections and already running clients.
    while (true)
    {
    	if (poll(&this->pollFds[0], this->pollFds.size(), -1) < 0)
    	{
    		Log::msgServer(ERROR, "Poll error");
    		continue;
    	}
        for (size_t i = 0; i < this->pollFds.size(); ++i)
        {
        	if (this->pollFds[i].revents == 0)
        	{
        		continue;
        	}

        	if (this->pollFds[i].revents & POLLHUP)
        	{
        		std::cout << "POLLHUP" << std::endl;
        		break;
        	}

            if (this->pollFds[i].revents & POLLIN)
            {
	            if (this->pollFds[i].fd == this->fd)
                {
                	connectClient();
                	break;
                }

            	std::cout << "existing client handling" << std::endl;
            	// Data from an existing client
            	char buffer[INPUT_BUFFER_SIZE];
            	const ssize_t bytesRead = recv(this->pollFds[i].fd, buffer, sizeof(buffer), 0);
            	if (bytesRead <= 0)
            	{
            		// Client disconnected or error
            		Log::msgServer(ERROR, "CLIENT", this->pollFds[i].fd, HANDLE_CLIENT_FAIL);
            		close(this->pollFds[i].fd);
            		this->pollFds[i].fd = -1; // Mark for removal
            	}
            	else
            	{
            		// Process data
            		buffer[bytesRead] = '\0';
            		Log::msgServer(INFO, "CLIENT", this->pollFds[i].fd, "[MESSAGE] " + std::string(buffer));
            	handleCommands(clients[i], buffer);
                }
            }
        }

        // Remove closed client sockets.
        for (std::vector<pollfd>::iterator it = this->pollFds.begin(); it != this->pollFds.end(); )
        {
            if (it->fd == -1)
            {
                it = this->pollFds.erase(it);
            }
        	else
        	{
                ++it;
            }
        }
    }
}

void Server::connectClient()
{
	socklen_t addrLen = sizeof(this->address);
	const int clientSocket = accept(this->fd, reinterpret_cast<sockaddr*>(&this->address), &addrLen);
	if (clientSocket < 0)
	{
		throw std::runtime_error(ERROR ACCEPT_FAIL);
	}

	Client client(clientSocket, POLLIN, 0);
	this->pollFds.push_back(client.getPollFd());
	this->clients.push_back(client);

	// Set the client socket to non-blocking mode.
	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1)
	{
		close(clientSocket);
		throw std::runtime_error(ERROR F_SETFL_FAIL);
	}

	if (!authenticateClient(client))
	{
		return;
	}
	Log::msgServer(INFO, "CLIENT", clientSocket, AUTHENTICATE_CLIENT_SUCCESS);
	// this->pollFds.push_back(client.getPollFd());
	// this->clients.push_back(client);
	// handleClientPrompt(clientSocket);
}


// TODO: implement certain amount of time that client has to send authentication data.
bool Server::authenticateClient(Client& client) const
{
	while (client.getPassword() != this->password || client.getNickname().empty() || client.getUsername().empty())
	{
		char buffer[INPUT_BUFFER_SIZE] = {};
		const ssize_t bytesRead = recv(client.getSocket(), buffer, sizeof(buffer) - 1, 0);
		if (bytesRead <= 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				continue;
			}
			Log::msgServer(ERROR, "CLIENT", client.getSocket(), HANDLE_CLIENT_FAIL);
			close(client.getSocket());
			return false;
		}
		buffer[bytesRead] = '\0';
		try
		{
			handleCommands(client, buffer);
		}
		catch (const std::exception& e)
		{
			Log::msgServer(ERROR,e.what());
		}
	}

	// TODO: in the first reply check what exactly is hostname (3rd arg), because it should be hostname of the client.
	// In the 4th reply check what exactly available modes (3rd, 4th arg) should it send to clients if any.
	reply(client, 001, Utils::anyToVec(client.getNickname(), client.getUsername(), this->name));
	reply(client, 002, Utils::anyToVec(this->name, this->version));
	reply(client, 003, Utils::anyToVec(this->creationDate));
	reply(client, 004, Utils::anyToVec(this->name, this->version, this->availableUserModes,
		this->availableChannelModes));
	return true;
}

void Server::handleCommands(Client& client, const std::string& buffer) const
{
	// // Data from an existing client
	// char buffer[INPUT_BUFFER_SIZE];
	// const ssize_t bytesRead = recv(this->pollFds[i].fd, buffer, sizeof(buffer), 0);
	// if (bytesRead <= 0)
	// {
	// 	// Client disconnected or error
	// 	Log::msgServer(ERROR, "CLIENT", this->pollFds[i].fd, HANDLE_CLIENT_FAIL);
	// 	close(this->pollFds[i].fd);
	// 	this->pollFds[i].fd = -1; // Mark for removal
	// }
	// else
	// {
	// 	// Process data
	// 	buffer[bytesRead] = '\0';
	// 	Log::msgServer(INFO, "CLIENT", this->pollFds[i].fd, "[MESSAGE] " + std::string(buffer));
	// }
	ClientTranslator translator(buffer);
	translator.fetchCommands(*this, client);

	std::cout << buffer << std::endl;
	std::map<std::string, std::vector<std::string> > commands = translator.getCommands();
	for (std::map<std::string, std::vector<std::string> >::iterator it = commands.begin(); it != commands.end(); ++it)
	{
		// TODO: check if there are specific error responses if there is something wrong with any of these commands.
		if (!client.getCapEnd() && it->first == "CAP" && !it->second.empty())
		{
			if (it->second[0] == "END")
			{
				client.setCapEnd(true);
				continue;
			}
			if (it->second[0] == "LS")
			{
				// TODO: check if server should send any capabilities or empty.
				// There is no CAP LS response in replies as this negotiation protocol is part of IRCv3 extensions.
				const std::string capResponse = ":server CAP * LS :\r\n";
				send(client.getSocket(), capResponse.c_str(), capResponse.size(), 0);
			}
		}
		else if (it->first == "PASS")
		{
			client.setPassword(it->second[0]);
		}
		else if (it->first == "NICK")
		{
			client.setNickname(it->second[0]);
		}
		else if (it->first == "USER")
		{
			// TODO: if less than 5 tokens, not enough information, return something to client.

			client.setUsername(it->second[0]);

			// TODO: handle realname.
			// It must be noted that realname parameter must be the last parameter,
			// because it may contain space characters and must be prefixed with a
			// colon (':') to make sure this is recognised as such.
		}
		else if (it->first == "JOIN")
		{
			// TODO: handle this command properly. This is just a test code.
			reply(client, 332, Utils::anyToVec(it->second[0]));
		}
	}
	// std::cout << "Client info:\n" << "socket: " << client.getSocket() << "\npassword: " << client.getPassword()
	// << "\nnickname: " << client.getNickname() << "\nusername: " << client.getUsername() << "\ncap status: "
	// << client.getCapEnd() << std::endl;
}

/**
 * @brief Handles communication with a connected client.
 *
 * This function enters a loop to continuously read data from the client socket, log the received messages,
 * and echo the messages back to the client. If an error occurs or the client disconnects, the function logs
 * an error message, closes the client socket, and breaks out of the loop.
 *
 * It performs the following steps:
 * 1. Reads data from the client socket into a buffer.
 * 2. Checks if the read operation was successful. If not, logs an error message, closes the client socket, and exits
 * the loop.
 * 3. Null-terminates the buffer and logs the received message.
 * 4. Sends the received message back to the client.
 *
 * @param clientSocket The socket file descriptor for the client connection.
 */
// TODO: handleClientPrompt and handleCommands might overlap, also need to understand how exactly handle the buffer.
void Server::handleClientPrompt(const int clientSocket)
{
	while (true)
	{
		char buffer[INPUT_BUFFER_SIZE] = {};
		const ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
		std::cout << "handleClientPrompt recv executed, buffer: " << buffer << std::endl;
		if (bytesRead <= 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				continue;
			}
			Log::msgServer(ERROR, "CLIENT", clientSocket, HANDLE_CLIENT_FAIL);
			close(clientSocket);
			break;
		}
		buffer[bytesRead] = '\0';
		Log::msgServer(INFO, "CLIENT", clientSocket, "[MESSAGE] " + std::string(buffer));
		break;
		// send(clientSocket, buffer, bytesRead, 0);
	}
}

void Server::reply(const Client& client, const int replyCode, const std::vector<std::string>& args) const
{
	if (this->replies.find(replyCode) != this->replies.end())
	{
		std::ostringstream oss;
		oss << ":" << this->name << " " << replyCode << " " << client.getNickname() << " :"
		<< this->replies.at(replyCode)(args) << "\r\n";
		const std::string reply = oss.str();
		send(client.getSocket(), reply.c_str(), reply.size(), 0);
	}
}

std::string Server::rplWelcome(const std::vector<std::string>& args)
{
	return "Welcome to the Internet Relay Network " + args[0] + "!" + args[1] + "@" + args[2];
}

std::string Server::rplYourHost(const std::vector<std::string>& args)
{
	return "Your host is " + args[0] + ", running version " + args[1];
}

std::string Server::rplCreated(const std::vector<std::string>& args)
{
	return "This server was created " + args[0];
}

std::string Server::rplMyInfo(const std::vector<std::string>& args)
{
	return args[0] + " " + args[1] + " " + args[2] + " " + args[3];
}

std::string Server::rplNoTopic(const std::vector<std::string>& args)
{
	return args[0] + " :No topic is set";
}


std::string Server::errUnknownCommand(const std::vector<std::string>& args)
{
	return args[0] + " :Unknown command";
}

std::string Server::errChannelIsFull(const std::vector<std::string>& args)
{
	return args[0] + " :Cannot join channel (+1)";
}