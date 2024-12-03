#include "../include/Client.h"

#include "Log.h"

Client::Client(const int fd) : fd(fd), welcomeRepliesSent(false), channelsJoined(0)
{

}

Client::Client() : fd(), welcomeRepliesSent(false), channelsJoined(0)
{

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

void Client::setRealname(const std::string& realname)
{
	this->realname = realname;
}

void Client::setWelcomeRepliesSent(const bool truefalse)
{
	this->welcomeRepliesSent = truefalse;
}

void Client::setChannelsJoined(const int num)
{
	this->channelsJoined = num;
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

int Client::getChannelsJoined() const
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

bool Client::getWelcomeRepliesSent() const
{
	return this->welcomeRepliesSent;
}
