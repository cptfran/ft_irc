#pragma once

#include <string>
#include <vector>
#include <netinet/in.h>

#define REGISTERED_PORT_MIN 1024
#define REGISTERED_PORT_MAX 49151

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
	std::vector<int> clientSockets;
	std::string password;

	bool authenticateClient(int clientSocket) const;
	static void handleClient(int clientSocket);
};