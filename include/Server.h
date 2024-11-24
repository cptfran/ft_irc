#pragma once

#include <map>
#include "Client.h"
#include "commands/Command.h"
#include <vector>

// Ports range:
#define REGISTERED_PORT_MIN 1024
#define REGISTERED_PORT_MAX 49151

// Client message buffer size:
#define INPUT_BUFFER_SIZE 512

class Server
{
public:
	Server(const std::string& name, const std::string& version, const std::string& password, int port);
	~Server();

	void setVersion(const std::string& version);

	std::string getName() const;
	std::string getPassword() const;
	std::map<int, Client> getClients() const;

	void run();
	void stop();

	void handleNicknameCollision(const std::string& newClientNickname);

private:
	static Server* instance;
	bool running;
	int fd;
	std::string name;
	std::string version;
	std::string password;
	std::string creationDate;
	std::string availableUserModes;
	std::string availableChannelModes;
	std::map<int, Client> clients;
	std::vector<pollfd> pollFds;
	std::map<std::string, Command*> validCommands;

	static void signalHandler(int signum);

	void initSocket(int port);

	// Client connection.
	void connectClient();
	void disconnectClient(Client& client);

	// Handling new requests of already connected client.
	void handleCommands(Client& client, const std::string& buffer);
	void handleClientPrompt(Client& client);
};