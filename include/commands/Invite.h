#pragma once

#include "Command.h"

class Invite: public Command
{
public:
    Invite();
    ~Invite();

    void execute(Server& server, Client& client, const std::vector<std::string>& args) const;
};