#pragma once

#include "Command.h"

class Channel;

class Mode: public Command
{
public:
    Mode();
    ~Mode();

    void execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const;

private:
    void handleUserModes(Client& requester, const std::string& nickname, ConfigManager& configManager,
        const std::vector<std::string>& args) const;
    void sendCurrentChannelModes(const std::string& serverName, const Channel& channel, const Client& requester) const;
    void editChannelModes(const std::vector<std::string>& args, const Client& requester, ConfigManager& configManager,
        Channel& channel) const;
    void handleKeyMode(Channel& channel, const std::string& action, const std::vector<std::string>& args, size_t& argsI,
        const Client& requester, const std::string& serverName, char mode) const;
    void handleOperatorMode(Channel& channel, const std::string& action, const std::vector<std::string>& args,
        size_t& argsI, const Client& requester, const std::string& serverName, char mode) const;
    void handleUserLimitMode(Channel& channel, const std::string& action, const std::vector<std::string>& args,
        size_t& argsI, const Client& requester, const std::string& serverName, char mode) const;
};
