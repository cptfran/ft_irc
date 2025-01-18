#pragma once

// Client message buffer size:
#define INPUT_BUFFER_SIZE 512
#define HOSTNAME_BUFFER_SIZE 1024

// TODO: change it to 60
#define TIME_FOR_CLIENT_TO_REGISTER 60000

#include "manager/ChannelManager.h"
#include "manager/ClientManager.h"
#include "manager/CommandManager.h"
#include "manager/ConfigManager.h"
#include "manager/ConnectionManager.h"

// TODO: add more logs of the server, so it logs whenever something imporant happens, like channel created etc.
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

	void stop();

	static void signalHandler(int signum);

	void handleTimeouts();

	friend class Command;
};
