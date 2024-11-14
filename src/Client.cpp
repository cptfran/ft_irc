#include "../include/Client.h"

Client::Client(const int fd) : fd(fd), welcomeRepliesSent(false), capEnd(false)
{

}

Client::~Client()
{

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

bool Client::getCapEnd() const
{
	return this->capEnd;
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

void Client::setWelcomeRepliesSent(const bool truefalse)
{
	this->welcomeRepliesSent = truefalse;
}

void Client::setCapEnd(const bool truefalse)
{
	this->capEnd = truefalse;
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
