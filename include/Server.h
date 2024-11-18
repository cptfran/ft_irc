#pragma once

#include <map>
#include "Client.h"
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

	void init(int port);
	void run();

private:
	typedef std::string (*ReplyFunction)(const std::vector<std::string>&);

	int fd;
	std::string name;
	std::string version;
	std::string password;
	std::string creationDate;
	std::string availableUserModes;
	std::string availableChannelModes;
	std::map<int, Client> clients;
	std::vector<pollfd> pollFds;
	std::vector<std::string> commandList;

	// New client connection.
	void connectClient();

	// Handling new requests of already connected client.
	void handleCommands(Client& client, const std::string& buffer) const;
	void handleClientPrompt(Client& client) const;
	void executeCommand(Client& client, const std::string& command, const std::vector<std::string>& args) const;

	bool isCommandValid(const std::string& command) const;
	bool isRegistrationCommand(const std::string& command) const;

	// Reply functions.
	void reply(const Client& client, ReplyFunction func, const std::vector<std::string>& args) const;
	static std::string rplWelcome(const std::vector<std::string>& args); // 001
	static std::string rplYourHost(const std::vector<std::string>& args); // 002
	static std::string rplCreated(const std::vector<std::string>& args); // 003
	static std::string rplMyInfo(const std::vector<std::string>& args); // 004
	static std::string rplPong(const std::vector<std::string>& args); // none
	static std::string rplCap(const std::vector<std::string>& args); // none
	static std::string rplNoTopic(const std::vector<std::string>& args); // 331
	static std::string errUnknownCommand(const std::vector<std::string>& args); // 421
	static std::string errNotRegistered(const std::vector<std::string>& args); // 451
	static std::string errPasswdMismatch(const std::vector<std::string>& args); // 464
	static std::string errChannelIsFull(const std::vector<std::string>& args); // 471
};