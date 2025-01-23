#pragma once

#include "manager/ChannelManager.h"
#include "manager/ClientManager.h"
#include "manager/CommandManager.h"
#include "manager/ConfigManager.h"
#include "manager/ConnectionManager.h"
#include <poll.h>

class Server
{
public:
	Server(const std::string& name, const std::string& version, const std::string& password, int port);
	~Server();

	void run();

private:
	static Server* instance;
	bool running;
	ChannelManager channelManager;
	ClientManager clientManager;
	CommandManager commandManager;
	ConfigManager configManager;
	ConnectionManager connectionManager;
	std::vector<pollfd> pollFds;
	std::vector<pollfd> pollFdsToAdd;
	std::vector<pollfd> pollFdsToDelete;

	void stop();

	static void signalHandler(int signum);

	void eventHandler();
	void timeoutHandler();
	void clientJoinHandler();
	void clientRequestHandler(int fd);
	void clientRemovalHandler(int fd);

	friend class Command;
};
