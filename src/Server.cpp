#include "../include/Server.h"
#include "../include/Logging.h"
#include <stdexcept>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <iostream>
#include <bits/stl_algo.h>


Server::Server(const std::string& password) : fd(-1), password(password)
{
	std::memset(&this->address, 0, sizeof(this->address));
}

Server::~Server()
{
	for (std::vector<int>::iterator it = clientSockets.begin(); it != clientSockets.end(); ++it)
	{
		close(*it);
	}
	if (this->fd != -1)
	{
		close(this->fd);
	}
}

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

void Server::run()
{
	logMessage(INFO, SERVER_RUN);
	while (true)
	{
		socklen_t addrLen = sizeof(this->address);
		const int newSocket = accept(this->fd, reinterpret_cast<sockaddr*>(&this->address), &addrLen);
		if (newSocket < 0)
		{
			logMessage(ERROR, ACCEPT_FAIL);
			continue;
		}
		if (!authenticateClient(newSocket))
		{
			close(newSocket);
			logMessage(ERROR, AUTHENTICATE_CLIENT_FAIL);
		}
		else
		{
			logMessage(INFO, "CLIENT", newSocket, AUTHENTICATE_CLIENT_SUCCESS);
			clientSockets.push_back(newSocket);
			handleClient(newSocket);
		}
	}
}

bool Server::authenticateClient(const int clientSocket) const
{
	char buffer[1024];
	std::string receivedPassword;
	const std::string prompt = "Enter password: ";
	send(clientSocket, prompt.c_str(), prompt.size(), 0);
	const int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
	if (bytesRead > 0)
	{
		receivedPassword = std::string(buffer, bytesRead);
		receivedPassword.erase(std::remove(receivedPassword.begin(), receivedPassword.end(), '\n'), receivedPassword.end());
	}
	return receivedPassword == this->password;
}

void Server::handleClient(int clientSocket)
{
	while (true)
	{
		char buffer[1024] = "Message";
		const int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (bytesRead <= 0)
		{
			logMessage(ERROR, "CLIENT", clientSocket, HANDLE_CLIENT_FAIL);
			close(clientSocket);
			break;
		}
		buffer[bytesRead - 1] = '\0';
		logMessage(INFO, "CLIENT", clientSocket, "[MESSAGE] " + std::string(buffer));
		// send(clientSocket, buffer, bytesRead, 0);
	}
}