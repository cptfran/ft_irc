#pragma once

#include <sys/poll.h>
#include <iostream>

using namespace std;

/**
 * A client is anything connecting to a server that is not another
 * server. Each client is distinguished from other clients by a unique
 * nickname having a maximum length of nine (9) characters. See the
 * protocol grammar rules for what may and may not be used in a
 * nickname. In addition to the nickname, all servers must have the
 * following information about all clients: the real name of the host
 * that the client is running on, the username of the client on that
 * host, and the server to which the client is connected.
 */
class Client
{
public:
	Client(int fd, short int events, short int revents);
	~Client();

	int getSocket() const;
	pollfd getPollFd() const;
	std::string& getNickname() const;
	std::string& getUsername() const;

	bool isPasswordCorrect();

private:
	Client();

	int socket;
	pollfd pollFd;
	std::string nickname;
	std::string username;
};
