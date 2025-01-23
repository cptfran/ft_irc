#include "data/Client.h"
#include "core/Log.h"
#include <ctime>
#include <unistd.h>

Client::Client(const int fd) : fd(fd), passwordAccepted(false), timeConnected(std::time(0)), welcomeRepliesSent(false),
	channelsJoined(0), invisible(false)
{

}

Client::Client() : fd(), passwordAccepted(false), timeConnected(std::time(0)), welcomeRepliesSent(false),
	channelsJoined(0), invisible(false)
{

}

bool Client::operator==(const Client& toCompare) const
{
	return this->fd == toCompare.fd &&
					   this->passwordAccepted == toCompare.passwordAccepted &&
					   this->nickname == toCompare.nickname &&
					   this->username == toCompare.username &&
					   this->realname == toCompare.realname &&
					   this->welcomeRepliesSent == toCompare.welcomeRepliesSent &&
					   this->channelsJoined == toCompare.channelsJoined;
}

Client::~Client()
{

}

void Client::setPasswordAccepted(bool accepted)
{
	this->passwordAccepted = accepted;
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

bool Client::getPasswordAccepted() const
{
	return this->passwordAccepted;
}

const std::string& Client::getNickname() const
{
	return this->nickname;
}

const std::string& Client::getUsername() const
{
	return this->username;
}

const std::string& Client::getHostname() const
{
	return this->hostname;
}

const std::string& Client::getRealname() const
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

bool Client::registered() const
{
	if (!this->passwordAccepted || this->nickname.empty() || this->username.empty())
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

/**
 * @brief Extracts a complete message from the received buffer.
 * 
 * This method searches for the first occurrence of the delimiter "\r\n" in the
 * receive buffer. If found, it extracts the complete message up to and including
 * the delimiter, removes it from the buffer, and returns the message. If the
 * delimiter is not found, it returns an empty string.
 * 
 * @return A complete message from the receive buffer, or an empty string if no complete message is found.
 */
const std::string Client::departCompleteMsgFromBuffer()
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