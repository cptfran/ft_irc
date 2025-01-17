#pragma once

#include "manager/ChannelManager.h"
#include "manager/ClientManager.h"
#include "manager/CommandManager.h"
#include "manager/ConnectionManager.h"
#include "Config.h"

// Ports range:
#define REGISTERED_PORT_MIN 1024
#define REGISTERED_PORT_MAX 49151

// Client message buffer size:
#define INPUT_BUFFER_SIZE 512
#define HOSTNAME_BUFFER_SIZE 1024

// TODO: change it to 60
#define TIME_FOR_CLIENT_TO_REGISTER 60000

class Channel;

// TODO: add more logs of the server, so it logs whenever something imporant happens, like channel created etc.
class Server
{
public:
	Server(const std::string& name, const std::string& version, const std::string& password, int port);
	~Server();

private:
	static Server* instance;
	bool running;
	Config config;
	ChannelManager channelManager;
	ClientManager clientManager;
	CommandManager commandManager;
	ConnectionManager connectionManager;

	void run();
	void stop();

	static void signalHandler(int signum);

	void Server::handleTimeouts();
};

//class Server 
//{
//public:
//	void run();
//	void stop();
//
//private:
//	std::string name;
//	std::string version;
//	std::string password;
//	std::string creationDate;
//
//	ConnectionManager connectionManager;
//	ClientManager clientManager;
//	ChannelManager channelManager;
//	CommandManager commandManager;
//
//	void initSignalHandlers();
//};