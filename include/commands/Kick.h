#pragma once

#include "Command.h"

// TODO: implement like in new rfc?..
class Kick: public Command
{
public:
    Kick();
    ~Kick();

    void execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const;

private:
    void kickUser(const std::vector<std::string>& args, Channel& channel, const Client& requester, 
        const Manager& serverManager) const;
};