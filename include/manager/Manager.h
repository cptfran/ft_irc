#pragma once

#include "ChannelManager.h"
#include "ClientManager.h"
#include "ConfigManager.h"
#include "ConnectionManager.h"

class CommandManager;

class Manager
{
public:
    Manager(ChannelManager& channelManager, ClientManager& clientManager, CommandManager& commandManager,
        ConfigManager& configManager, ConnectionManager& connectionManager);
    ~Manager();

    ChannelManager& getChannelManager() const;
    ClientManager& getClientManager() const;
    CommandManager& getCommandManager() const;
    ConfigManager& getConfigManager() const;
    ConnectionManager& getConnectionManager() const;

    void handleNicknameCollision(int newClientFd, const std::string& newClientNickname);

private:
    ChannelManager& channelManager;
    ClientManager& clientManager;
    CommandManager& commandManager;
    ConfigManager& configManager;
    ConnectionManager& connectionManager;

    Manager(const Manager& toCopy);
    Manager& operator=(const Manager& toAssign);
};