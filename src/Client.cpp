#include "../include/Client.h"
#include <unistd.h>

Client::Client(const int fd, const short int events, const short int revents) : socket(fd), pollFd()
{
	this->pollFd.fd = fd;
	this->pollFd.events = events;
	this->pollFd.revents = revents;
}

Client::~Client()
{
	if (this->socket != -1)
	{
		close(this->socket);
	}
}

int Client::getSocket() const
{
	return this->socket;
}


pollfd Client::getPollFd() const
{
	return this->pollFd;
}
