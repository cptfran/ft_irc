#pragma once

#include "Command.h"

class Topic: public Command
{
public:
    Topic();
    ~Topic();

    void execute(Server& server, Client& client, const std::vector<std::string>& args) const;
    void setTopic(const std::vector<std::string>& args, const std::string& requestorNickname, Channel& channel,
        const std::string& serverName) const;
};