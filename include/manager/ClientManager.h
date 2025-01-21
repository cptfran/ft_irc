#pragma once

#include <map>
#include <netdb.h>
#include "data/Client.h"

class ClientManager
{
public:
    ClientManager();
    ~ClientManager();

    void addClient(int clientFd);
    void queueClientToDelete(int clientFd);
    void deleteQueuedClients();

    Client& getClientByFd(int fd);
    Client* getClientByNickname(const std::string& nickname);
    const std::map<int, Client>& getClients() const;
    std::string findClientHostname(sockaddr_in& addr, socklen_t addrLen, int clientFd) const;

private:
    std::map<int, Client> clients;
    std::vector<int> fdsToDelete;

    ClientManager(const ClientManager& toCopy);
    ClientManager& operator=(const ClientManager& toAssign);
};