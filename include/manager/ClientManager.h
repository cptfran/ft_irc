#pragma once

#include <map>
#include "client/Client.h"
#include <netdb.h>

class ClientManager
{
public:
    ClientManager();
    ~ClientManager();

    void addClient(int clientFd, const Client& client);
    void removeClient(int clientFd);

    Client* findClientByNickname(const std::string& nickname);
    std::map<int, Client> getClients() const;
    std::string getClientHostname(sockaddr_in& addr, socklen_t addrLen, int clientFd) const;

private:
    std::map<int, Client> clients;

    ClientManager(const ClientManager& toCopy);
    ClientManager& operator=(const ClientManager& toAssign);
};