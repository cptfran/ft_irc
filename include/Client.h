#pragma once

#include <iostream>

// Maxium number of channels, client can join:
#define CHANNELS_MAX 10

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
	explicit Client(int fd);
	bool operator==(const Client& toCompare) const;
	~Client();

	int getFd() const;
	std::string getPassword() const;
	std::string getNickname() const;
	std::string getUsername() const;

	void setPassword(const std::string& password);
	void setNickname(const std::string& nickname);
	void setUsername(const std::string& username);
	void setRealname(const std::string& realname);
	void setWelcomeRepliesSent(bool truefalse);
	void setNumChannelsJoined(int num);

	bool registered(const std::string& serverPassword) const;
	bool getWelcomeRepliesSent() const;
	int getNumChannelsJoined() const;

private:
	int fd;
	std::string password;
	std::string nickname;
	std::string username;
	std::string realname;
	bool welcomeRepliesSent;
	int channelsJoined;

	Client();
};
