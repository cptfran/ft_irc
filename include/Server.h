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
	typedef std::string (*ReplyFunction)(const std::vector<std::string>&);

	Server(const std::string& name, const std::string& version, const std::string& password, int port);
	~Server();

	void setVersion(const std::string& version);

	std::string getName() const;
	std::string getPassword() const;

	void run();
	void stop();

	// TODO: maybe make Replier class so it is more flexible and more single responsibility approach.
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

	// New client connection.
	void connectClient();

	// Handling new requests of already connected client.
	void handleCommands(Client& client, const std::string& buffer) const;
	void handleClientPrompt(Client& client) const;
	void executeCommand(Client& client, const std::string& command, const std::vector<std::string>& args) const;

	bool isRegistrationCommand(const std::string& command) const;
};