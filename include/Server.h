#pragma once

#include "Client.h"
#include <string>
#include <vector>
#include <netinet/in.h>

#define REGISTERED_PORT_MIN 1024
#define REGISTERED_PORT_MAX 49151
#define INPUT_BUFFER_SIZE 512

class Server
{
public:
	explicit Server(const std::string& password);
	~Server();

	void init(int port);
	void run();

private:
	int fd;
	sockaddr_in address;
	std::string password;
	std::vector<Client> clients;
	std::vector<pollfd> pollFds;

	bool authenticateClient(int clientSocket) const;
	static void handleClient(int clientSocket);
	static void handleCapLs(int clientSocket);
	static std::string extractPassword(const std::string& buffer);
	void connectClient();
	// void parser();
};