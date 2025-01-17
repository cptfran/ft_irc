#pragma once

#include <data/Client.h>
#include <commands/Command.h>
#include <map>

class CommandManager 
{
public:
    CommandManager();
    ~CommandManager();

    void executeCommands(Client& client, const std::string& serverPassword);
    void executeCommand(Client& client, const std::string& buffer, const std::string& serverPassword);

private:
    std::map<std::string, Command*> validCommands;

    CommandManager(const CommandManager& toCopy);
    CommandManager& operator=(const CommandManager& toAssign);
};
