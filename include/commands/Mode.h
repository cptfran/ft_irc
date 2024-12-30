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
    void sendCurrentChannelModes(const std::string& serverName, const Channel& channel, const Client& requestor) const;
    void editChannelModes(const std::vector<std::string>& args, const Client& requestor, const std::string& serverName,
        Channel& channel) const;
    void handleKeyMode(Channel& channel, const std::string& action, const std::vector<std::string>& args, size_t& argsI,
        const Client& requestor, const std::string& serverName, char mode) const;
    void handleOperatorMode(Channel& channel, const std::string& action, const std::vector<std::string>& args,
        size_t& argsI, const Client& requestor, const std::string& serverName, char mode) const;
    void handleUserLimitMode(Channel& channel, const std::string& action, const std::vector<std::string>& args,
        size_t& argsI, const Client& requestor, const std::string& serverName, char mode) const;
};
