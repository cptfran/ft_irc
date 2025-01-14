#pragma once

#include "Command.h"

class Channel;

class Privmsg : public Command
{
public:
    Privmsg();
    ~Privmsg();

    void execute(Server& server, Client& requester, const std::vector<std::string>& args) const;

private:
    std::vector<std::pair<Client, std::string> > findMatchingTargets(const Client& requester, Server& server, 
        const std::string& targets) const;
    std::vector<std::pair<Client, std::string> > getTargetsFromServer(const Client& requester, Server& server, 
        const std::string& extrTarget) const;
    void sendMessagesToTargets(const Client& client, const std::vector<std::pair<Client, std::string> >& foundTargets,
        const std::string& message) const;
    std::vector<std::pair<Client, std::string> > getChannelTargets(const Client& requester, Server& server, 
        const std::string& extrTarget) const;
    std::vector<std::pair<Client, std::string> > getHostnameTargetsByWildcard(const Client& requester, 
        const Server& server, const std::string& extrTarget) const;
    std::vector<std::pair<Client, std::string> > getServerTargetsByWildcard(const Client& requester,
        const Server& server, const std::string& extrTarget) const;
    std::vector<std::pair<Client, std::string> > getUserTargets(const Client& requester, const Server& server, 
        const std::string& extrTarget) const;
};
