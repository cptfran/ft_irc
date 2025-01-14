#pragma once

#include "Command.h"

class Kick: public Command
{
public:
    Kick();
    ~Kick();

    void execute(Server& server, Client& requester, const std::vector<std::string>& args) const;

private:
    void kickUser(const std::vector<std::string>& args, Channel& channel, const Client& requester, Server& server) 
        const;
};