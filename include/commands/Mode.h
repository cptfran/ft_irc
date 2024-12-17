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
    void Mode::sendCurrentChannelModes(const std::string& serverName, const Channel* channel, const int clientFd) const;
    void editChannelModes(const std::vector<std::string>& args, int clientFd, const std::string& serverName,
        Channel* channel) const;
};
