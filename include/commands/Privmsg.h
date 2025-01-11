#pragma once

#include "Command.h"

class Channel;

class Privmsg : public Command
{
public:
    Privmsg();
    ~Privmsg();

    void execute(Server& server, Client& client, const std::vector<std::string>& args) const;
private:
    bool validateArgs(Server& server, Client& client, const std::vector<std::string>& args) const;
    std::vector<Client> findMatchingTargets(Client& client, Server& server, const std::string& targets) const;
    std::vector<Client> getTargetsFromServer(Client& requester, Server& server, const std::string& extrTarget) const;
    void sendMessagesToTargets(const Client& client, const std::vector<Client>& foundTargets,
        const std::string& message) const;
    std::vector<Client> getChannelTargets(Server& server, const std::string& extrTarget) const;
    std::vector<Client> getHostnameTargetsByWildcard(Client& requester, Server& server, const std::string& extrTarget) 
        const;
    std::vector<Client> getServerTargetsByWildcard(Client& requester, Server& server, const std::string& extrTarget) 
        const;
    std::vector<Client> getUserTargets(Client& requester, Server& server, const std::string& extrTarget) const;
    void parseUserTarget(const std::string& extrTarget, std::string& nickname, std::string& username, 
        std::string& hostname, std::string& serverName) const;
};
