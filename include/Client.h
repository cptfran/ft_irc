#pragma once

#include <sys/poll.h>
#include <iostream>

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
	// Client(int fd, short int events, short int revents);
	explicit Client(int fd);
	~Client();

	int getFd() const;
	std::string getPassword() const;
	std::string getNickname() const;
	std::string getUsername() const;

	void setPassword(const std::string& password);
	void setNickname(const std::string& nickname);
	void setUsername(const std::string& username);
	void setWelcomeRepliesSent(bool truefalse);

	bool registered(const std::string& serverPassword) const;
	bool getWelcomeRepliesSent() const;

private:
	Client();

	int fd;
	std::string password;
	std::string nickname;
	std::string username;
	bool welcomeRepliesSent;
};
