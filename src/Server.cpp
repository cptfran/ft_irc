#include "../include/Server.h"

#include <cmath>

#include "../include/Logging.h"
#include <stdexcept>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <iostream>
#include <list>
#include <bits/stl_algo.h>
#include <poll.h>

/**
 * @brief Constructor for the Server class.
 *
 * This constructor initializes a Server object with a specified password.
 * It performs the following steps:
 * 1. Initializes the server's file descriptor (fd) to -1, indicating that no socket is currently open.
 * 2. Sets the server's password to the provided value.
 * 3. Clears the server's address structure by setting all its bytes to zero.
 *
 * @param password The password to be used for client authentication.
 */
Server::Server(const std::string& password) : fd(-1), password(password)
{
	std::memset(&this->address, 0, sizeof(this->address));
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
 * This function enters an infinite loop where it continuously accepts new client connections.
 * For each new connection, it attempts to authenticate the client using the authenticateClient function.
 * If authentication fails, the connection is closed and an error message is logged.
 * If authentication succeeds, a success message is logged, a welcome message is sent to the client,
 * the client's socket is added to the list of client sockets, and the handleClient function is called to manage the
 * client.
 * The function logs messages at various stages to provide information about the server's operation and any errors
 * encountered.
 */
void Server::run()
{
    logMessage(INFO, SERVER_RUN);
    pollfd listen_fd = {};
    listen_fd.fd = this->fd;
    listen_fd.events = POLLIN;
    listen_fd.revents = 0;
    this->pollFds.push_back(listen_fd);
    while (true)
    {
    	if (poll(&this->pollFds[0], this->pollFds.size(), -1) < 0)
    	{
    		logMessage(ERROR, "Poll error");
    		continue;
    	}
        for (size_t i = 0; i < this->pollFds.size(); ++i)
        {
            if (this->pollFds[i].revents & POLLIN)
            {
                if (this->pollFds[i].fd == this->fd)
                {
                	connectClient();
                }
            	else
            	{
                   // TODO: create client class to handle multiple clients and mantain their state
            		// Data from an existing client
            		char buffer[1024];
            		int bytesRead = recv(this->pollFds[i].fd, buffer, sizeof(buffer), 0);
            		if (bytesRead <= 0)
            		{
            			// Client disconnected or error
            			logMessage(ERROR, "CLIENT", this->pollFds[i].fd, HANDLE_CLIENT_FAIL);
            			close(this->pollFds[i].fd);
            			this->pollFds[i].fd = -1; // Mark for removal
            		}
            		else
            		{
            			// Process data
            			buffer[bytesRead] = '\0';
            			logMessage(INFO, "CLIENT", this->pollFds[i].fd, "[MESSAGE] " + std::string(buffer));
            			send(this->pollFds[i].fd, buffer, bytesRead, 0); // Echo back
            		}
                }
            }
        }

        // Remove closed client sockets
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
		logMessage(ERROR, ACCEPT_FAIL);
		return;
	}
	if (!authenticateClient(clientSocket))
	{
		logMessage(ERROR, AUTHENTICATE_CLIENT_FAIL);
		close(clientSocket);
		return;
	}
	logMessage(INFO, "CLIENT", clientSocket, AUTHENTICATE_CLIENT_SUCCESS);
	Client client(clientSocket, POLLIN, 0);
	this->pollFds.push_back(client.getPollFd());
	this->clients.push_back(client);
	handleClient(clientSocket);
}


/**
 * @brief Authenticates a client by reading data from the client socket and extracting the password.
 *
 * This function handles the CAP LS command for the client and then enters a loop to read data from the client socket.
 * It reads data into a buffer and extracts the password using the extractPassword function.
 * If a password is extracted and it matches the server's stored password, the function returns true, indicating
 * successful authentication.
 * If no password is extracted or the extracted password does not match, the function continues reading until no more
 * data is available.
 * If the loop exits without a successful match, the function returns false.
 *
 * @param clientSocket The socket file descriptor for the client connection.
 * @return True if the client is successfully authenticated, false otherwise.
 */
// TODO: modify to handle not only cap ls, pass but also to verify hostname etc
bool Server::authenticateClient(const int clientSocket) const
{
	handleCapLs(clientSocket);
	int bytesRead = 1;
	while (bytesRead > 0)
	{
		char buffer[1024] = {};
		bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
		if (bytesRead > 0)
		{
			buffer[bytesRead] = '\0';
			std::cout << buffer << std::endl;
			std::string receivedPassword = extractPassword(buffer);
			if (!receivedPassword.empty())
			{
				return receivedPassword == this->password;
			}
		}
	}
	return false;
}

/**
 * @brief Extracts the password from a given buffer string.
 *
 * This function searches for the keyword "PASS " within the provided buffer string.
 * If the keyword is found, it extracts the substring following the keyword up to the next newline character.
 * If no newline character is found, it extracts up to the end of the buffer.
 * If the keyword is not found, it returns an empty string.
 *
 * @param buffer The input string containing the password keyword and the password.
 * @return The extracted password as a string. If the keyword is not found, returns an empty string.
 */
std::string Server::extractPassword(const std::string& buffer)
{
	const std::string passwordKeyword = "PASS ";
	const size_t passwordPosition = buffer.find(passwordKeyword);
	if (passwordPosition == std::string::npos)
	{
		return "";
	}
	const size_t passwordStart = passwordPosition + passwordKeyword.length();
	size_t passwordEnd = buffer.find('\n', passwordStart);
	if (passwordEnd == std::string::npos)
	{
		passwordEnd = buffer.length();
	}
	std::string password = buffer.substr(passwordStart, passwordEnd - 1 - passwordStart);
	return password;
}

/**
 * @brief Handles the CAP LS command from a client.
 *
 * This function reads data from the client socket to check for the "CAP LS" command.
 * If the command is found, it sends a CAP LS response back to the client.
 *
 * It performs the following steps:
 * 1. Reads data from the client socket into a buffer.
 * 2. Null-terminates the buffer and converts it to a string.
 * 3. Checks if the received command contains "CAP LS".
 * 4. If "CAP LS" is found, sends a CAP LS response to the client.
 *
 * @param clientSocket The socket file descriptor for the client connection.
 */
void Server::handleCapLs(const int clientSocket)
{
	char buffer[1024];
	const int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
	if (bytesRead <= 0)
	{
		logMessage(ERROR, "CLIENT", clientSocket, HANDLE_CLIENT_FAIL);
		close(clientSocket);
		return;
	}
	buffer[bytesRead] = '\0';
	std::cout << buffer << std::endl;
	const std::string receivedCommand = buffer;
	if (receivedCommand.find("CAP LS") != std::string::npos)
	{
		const std::string capResponse = ":server CAP * LS :\r\n";
		send(clientSocket, capResponse.c_str(), capResponse.size(), 0);
	}
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
void Server::handleClient(const int clientSocket)
{
	while (true)
	{
		char buffer[1024];
		const int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (bytesRead <= 0)
		{
			logMessage(ERROR, "CLIENT", clientSocket, HANDLE_CLIENT_FAIL);
			close(clientSocket);
			break;
		}
		buffer[bytesRead] = '\0';
		logMessage(INFO, "CLIENT", clientSocket, "[MESSAGE] " + std::string(buffer));
		send(clientSocket, buffer, bytesRead, 0);
	}
}
