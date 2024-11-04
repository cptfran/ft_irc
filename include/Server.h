#pragma once

#include "Client.h"
#include <string>
#include <vector>
#include <netinet/in.h>

#define REGISTERED_PORT_MIN 1024
#define REGISTERED_PORT_MAX 49151
#define INPUT_BUFFER_SIZE 512

using namespace std;

class Server
{
public:
	explicit Server(const string& password);
	~Server();

	void init(int port);
	void run();

private:
	int fd;
	sockaddr_in address;
	string password;
	vector<Client> clients;
	vector<pollfd> pollFds;

	void authenticateClient(Client& client) const;
	static void handleClient(int clientSocket);
	static void handleCapLs(int clientSocket);
	static string extractPassword(const string& buffer);
	void connectClient();
};