#pragma once

#include <client/Client.h>
#include <commands/Command.h>
#include <map>

class CommandManager 
{
public:
    CommandManager();
    ~CommandManager();

    void executeCommand(Client& client, const std::string& buffer);

private:
    std::map<std::string, Command*> validCommands;

    CommandManager(const CommandManager& toCopy);
    CommandManager& operator=(const CommandManager& toAssign);
};
