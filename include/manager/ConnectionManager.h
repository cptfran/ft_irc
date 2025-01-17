#pragma once

#include <sys/poll.h>
#include <vector>
#include <string>

class ConnectionManager 
{
public:
    ConnectionManager();
    ~ConnectionManager();

    void initSocket(int port);
    void runServer();
    void stopServer();
    void connectClient();
    void addNewClientsToPoll();
    void disconnectClient(int clientFd);
    void handleTimeouts();
    void handleNicknameCollision(int newClientFd, const std::string& newClientNickname);

private:
    bool serverRunning;
    std::vector<pollfd> pollFds;
    std::vector<pollfd> pollFdsToAdd;

    ConnectionManager(const ConnectionManager& toCopy);
    ConnectionManager& operator=(const ConnectionManager& toAssign);
};