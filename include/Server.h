#pragma once

#include <map>
#include "Client.h"
#include <vector>
#include <netinet/in.h>

// Ports range:
#define REGISTERED_PORT_MIN 1024
#define REGISTERED_PORT_MAX 49151

// Client message buffer size:
#define INPUT_BUFFER_SIZE 512

class Server
{
public:
	Server(const std::string& name, const std::string& version, const std::string& password);
	~Server();

	void setVersion(const std::string& version);

	void init(int port);
	void run();

	void reply(const Client& client, int replyCode, const std::vector<std::string>& args) const;

private:
	typedef std::string (*ReplyFunction)(const std::vector<std::string>&);

	int fd;
	std::string name;
	std::string version;
	std::string password;
	std::string creationDate;
	std::string availableUserModes;
	std::string availableChannelModes;
	std::vector<Client> clients;
	std::vector<pollfd> pollFds;
	std::map<int, ReplyFunction> replies;

	// New client connection.
	void connectClient();
	// bool authenticateClient(Client& client) const;

	// Handling new requests of already connected client.
	void handleCommands(Client& client, const std::string& buffer) const;
	void handleClientPrompt(Client& client);

	// Reply functions.
	static std::string rplWelcome(const std::vector<std::string>& args);
	static std::string rplYourHost(const std::vector<std::string>& args);
	static std::string rplCreated(const std::vector<std::string>& args);
	static std::string rplMyInfo(const std::vector<std::string>& args);
	static std::string rplNoTopic(const std::vector<std::string>& args);
	static std::string errUnknownCommand(const std::vector<std::string>& args);
	static std::string errChannelIsFull(const std::vector<std::string>& args);
};