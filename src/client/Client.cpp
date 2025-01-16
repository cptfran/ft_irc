#include "client/Client.h"
#include "server/Log.h"
#include <ctime>
#include <cstring>

Client::Client(const int fd) : fd(fd), timeConnected(std::time(0)), welcomeRepliesSent(false), channelsJoined(0)
{

}

Client::Client() : fd(), timeConnected(std::time(0)), welcomeRepliesSent(false), channelsJoined(0)
{

}

bool Client::operator==(const Client& toCompare) const
{
	return this->fd == toCompare.fd && this->password == toCompare.password && this->nickname == toCompare.nickname
		&& this->username == toCompare.username && this->realname == toCompare.realname
		&& this->welcomeRepliesSent == toCompare.welcomeRepliesSent && this->channelsJoined == toCompare.channelsJoined;
}

Client::~Client()
{

}

void Client::setPassword(const std::string& password)
{
	this->password = password;
}

void Client::setNickname(const std::string& nickname)
{
	this->nickname = nickname;
}

void Client::setUsername(const std::string& username)
{
	this->username = username;
}

void Client::setHostname(const std::string& hostname)
{
	this->hostname = hostname;
}


void Client::setRealname(const std::string& realname)
{
	this->realname = realname;
}

void Client::setWelcomeRepliesSent(bool sent)
{
	this->welcomeRepliesSent = sent;
}

void Client::setNumChannelsJoined(const int num)
{
	this->channelsJoined = num;
}

void Client::setInvisible(bool invisible)
{
	this->invisible = invisible;
}

void Client::addToBuffer(const std::string& str)
{
	this->recvBuffer += str;
}

int Client::getFd() const
{
	return this->fd;
}

std::string Client::getPassword() const
{
	return this->password;
}

std::string Client::getNickname() const
{
	return this->nickname;
}

std::string Client::getUsername() const
{
	return this->username;
}

std::string Client::getHostname() const
{
	return this->hostname;
}

std::string Client::getRealname() const
{
	return this->realname;
}

bool Client::isInvisible() const
{
	return this->invisible;
}

int Client::getNumChannelsJoined() const
{
	return this->channelsJoined;
}

bool Client::registered(const std::string& serverPassword) const
{
	if (this->password != serverPassword || this->nickname.empty() || this->username.empty())
	{
		return false;
	}
	return true;
}

time_t Client::getTimeConnected() const
{
	return this->timeConnected;
}

bool Client::getWelcomeRepliesSent() const
{
	return this->welcomeRepliesSent;
}

std::string Client::departCompleteMsgFromBuffer()
{
	// Find the position of "\r\n" in recvBuffer
	size_t endOfMsgPos = this->recvBuffer.find("\r\n");
	if (endOfMsgPos == std::string::npos)
	{
		return "";
	}

	// Extract the complete message
	std::string completeMsg = this->recvBuffer.substr(0, endOfMsgPos + 2);

	// Remove the extracted message from recvBuffer
	this->recvBuffer.erase(0, endOfMsgPos + 2);

	return completeMsg;
}