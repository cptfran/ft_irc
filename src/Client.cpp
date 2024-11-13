#include "../include/Client.h"
#include <unistd.h>

// Client::Client(const int fd, const short int events, const short int revents) : fd(fd), pollFd(), capEnd(false)
// {
	// this->pollFd.fd = fd;
	// this->pollFd.events = events;
	// this->pollFd.revents = revents;
// }

Client::Client(const int fd) : fd(fd), capEnd(false)
{

}

Client::~Client()
{
	// if (this->fd != -1)
	// {
		// close(this->fd);
	// }
}

int Client::getFd() const
{
	return this->fd;
}

// TODO: delete?
// pollfd Client::getPollFd() const
// {
	// return this->pollFd;
// }

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

void Client::setCapEnd(const bool truefalse)
{
	this->capEnd = truefalse;
}
