#pragma once

#include "Command.h"

class Channel;

class Privmsg : public Command
{
public:
    Privmsg();
    ~Privmsg();

    void execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const;

private:
    std::vector<std::pair<Client, std::string> > findMatchingTargets(const Client& requester, Manager& serverManager,
        const std::string& targets) const;
    std::vector<std::pair<Client, std::string> > getTargetsFromServer(const Client& requester, Manager& serverManager,
        const std::string& extrTarget) const;
    std::vector<std::pair<Client, std::string> > getChannelTargets(const Client& requester, Manager& serverManager,
        const std::string& extrTarget) const;
    std::vector<std::pair<Client, std::string> > getHostnameTargetsByWildcard(const Client& requester, 
        Manager& serverManager, const std::string& extrTarget) const;
    std::vector<std::pair<Client, std::string> > getServerTargetsByWildcard(const Client& requester,
        Manager& serverManager, const std::string& extrTarget) const;
    std::vector<std::pair<Client, std::string> > getUserTargets(const Client& requester, Manager& serverManager,
        const std::string& extrTarget) const;
};
