#pragma once

#include <sys/poll.h>
#include <vector>
#include <string>

class ConnectionManager 
{
public:
    ConnectionManager();
    ~ConnectionManager();

    int initSocket(int port);
    pollfd acceptNewClientConnection(int serverFd);
    void queueNewClientToAddToPoll(const pollfd pollFd);
    void addNewClientsToPoll(std::vector<pollfd>& pollFds);
    void queueClientToDeleteFromPoll(int fd);
    void deleteQueuedClientsFromPoll(std::vector<pollfd>& pollFds);

private:
    std::vector<pollfd> pollFdsToAdd;
    std::vector<int> fdsToDelete;

    ConnectionManager(const ConnectionManager& toCopy);
    ConnectionManager& operator=(const ConnectionManager& toAssign);
};