#pragma once

#include <sys/poll.h>
#include <vector>
#include <string>

class ConnectionManager 
{
public:
    ConnectionManager();
    ~ConnectionManager();

    std::vector<pollfd> getPollFds() const;

    const int initSocket(int port);
    void addServerToPoll(int fd);
    void acceptNewClientConnection(int fd);
    void addNewClientsToPoll();
    void queueClientToDeleteFromPoll(pollfd pollFd);
    void deleteQueuedClientsFromPoll();
    //void disconnectClient(int clientFd);
    void handleNicknameCollision(int newClientFd, const std::string& newClientNickname);

private:
    std::vector<pollfd> pollFds;
    std::vector<pollfd> pollFdsToAdd;
    std::vector<pollfd> pollFdsToDelete;

    ConnectionManager(const ConnectionManager& toCopy);
    ConnectionManager& operator=(const ConnectionManager& toAssign);
};