#pragma once

#include <iostream>

/**
 * A client is anything connecting to a server that is not another
 * server. Each client is distinguished from other clients by a unique
 * nickname having a maximum length of nine (9) characters. See the
 * protocol grammar rules for what may and may not be used in a
 * nickname. In addition to the nickname, all servers must have the
 * following Log::INFOrmation about all clients: the real name of the host
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
	bool getPasswordAccepted() const;
	const std::string& getNickname() const;
	const std::string& getUsername() const;
	const std::string& getHostname() const;
	const std::string& getRealname() const;
	bool registered() const;
	time_t getTimeConnected() const;
	bool getWelcomeRepliesSent() const;
	int getNumChannelsJoined() const;
	bool isInvisible() const;
	const std::string departCompleteMsgFromBuffer();

	void setPasswordAccepted(bool accepted);
	void setNickname(const std::string& nickname);
	void setUsername(const std::string& username);
	void setHostname(const std::string& hostname);
	void setRealname(const std::string& realname);
	void setWelcomeRepliesSent(bool sent);
	void setNumChannelsJoined(int num);
	void setInvisible(bool invisible);
	void addToBuffer(const std::string& str);

private:
	int fd;
	bool passwordAccepted;
	std::string nickname;
	std::string username;
	std::string hostname;
	std::string realname;
	time_t timeConnected;
	bool welcomeRepliesSent;
	int channelsJoined;
	bool invisible;
	std::string recvBuffer;

	Client();
};
