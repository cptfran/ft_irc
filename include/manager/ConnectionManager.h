#pragma once

#include <sys/poll.h>
#include <vector>
#include <string>

// Ports range:
#define REGISTERED_PORT_MIN 1024
#define REGISTERED_PORT_MAX 49151

class ConnectionManager 
{
public:
    ConnectionManager();
    ~ConnectionManager();

    std::vector<pollfd> getPollFds() const;

    int initSocket(int port);
    void addServerToPoll(int fd);
    void acceptNewClientConnection(int fd);
    void addNewClientsToPoll();
    void queueClientToDeleteFromPoll(pollfd pollFd);
    void deleteQueuedClientsFromPoll();
    //void disconnectClient(int clientFd);

private:
    std::vector<pollfd> pollFds;
    std::vector<pollfd> pollFdsToAdd;
    std::vector<pollfd> pollFdsToDelete;

    ConnectionManager(const ConnectionManager& toCopy);
    ConnectionManager& operator=(const ConnectionManager& toAssign);

    bool operator==(const pollfd& rhs) const;
};