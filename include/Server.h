#pragma once

#include <map>
#include "Client.h"
#include "commands/Command.h"
#include <vector>
#include <netinet/in.h>
#include <sys/poll.h>

// Ports range:
#define REGISTERED_PORT_MIN 1024
#define REGISTERED_PORT_MAX 49151

// Client message buffer size:
#define INPUT_BUFFER_SIZE 512
#define HOSTNAME_BUFFER_SIZE 1024

#define TIME_FOR_CLIENT_TO_REGISTER 15

class Channel;

// TODO: add more logs of the server, so it logs whenever something imporant happens, like channel created etc.
class Server
{
public:
	Server(const std::string& name, const std::string& version, const std::string& password, int port);
	~Server();

	void setVersion(const std::string& version);

	std::string getName() const;
	std::string getPassword() const;
	std::map<int, Client> getClients() const;
	Client* findClientByNickname(const std::string& nicknameToFind);

	void run();
	void stop();

	void handleNicknameCollision(int newClientFd, const std::string& newClientNickname);

	// Channels:
	Channel* getChannel(const std::string& channelName);
	std::string getAvailableChannelModes() const;
	std::string getAvailableUserModes() const;
	void addChannel(const Channel& channel);
	void deleteChannelIfEmpty(const Channel& channel);

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
	std::vector<Channel> channels;

	static void signalHandler(int signum);

	void initSocket(int port);

	// Client connection.
	void connectClient();
	std::string getClientHostname(sockaddr_in& addr, socklen_t addrLen, int clientFd) const;
	void disconnectClient(int clientFd);

	// Handling new requests of already connected client.
	void handleCommands(Client& client, const std::string& buffer);
	void handleClientPrompt(Client& client);

	void handleTimeouts();
};