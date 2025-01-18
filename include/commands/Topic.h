#pragma once

#include "Command.h"

class Topic: public Command
{
public:
    Topic();
    ~Topic();

    void execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const;
    void setTopic(const std::vector<std::string>& args, const Client& requester, Channel& channel,
        const std::string& serverName) const;
};