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
    std::vector<Client> findTargetsOnServer(Client& requester, Server& server,
        const std::string& extractedTarget) const;
};
