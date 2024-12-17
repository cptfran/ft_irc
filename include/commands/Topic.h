#pragma once

#include "Command.h"

class Topic: public Command
{
public:
    Topic();
    ~Topic();

    void execute(Server& server, Client& client, const std::vector<std::string>& args) const;
    void sendTopic(const Channel& channel, int requestorFd, const std::string& serverName) const;
    void setTopic(const std::vector<std::string>& args, Channel& channel, const std::string& serverName) const;
};