#pragma once

#include "Command.h"

class Channel;

class Mode: public Command
{
public:
    Mode();
    ~Mode();

    void execute(Server& server, Client& client, const std::vector<std::string>& args) const;

private:
    bool sendCurrentChannelModes(size_t argsSize, const std::string& serverName, const Channel* channel,
        int clientFd) const;
    void editChannelModes(const std::vector<std::string>& args, int clientFd, const std::string& serverName,
        Channel* channel) const;
};
