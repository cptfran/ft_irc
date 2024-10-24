#pragma once

#include <sys/poll.h>

class Client
{
public:
	Client(int fd, short int events, short int revents);
	~Client();

	pollfd getPollFd() const;

private:
	int socket;
	pollfd pollFd;

	Client();
};
