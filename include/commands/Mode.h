#pragma once

#include "Command.h"

class Mode: public Command
{
public:
    Mode();
    ~Mode();

    void execute(Server& server, Client& client, const std::vector<std::string>& args) const;
};