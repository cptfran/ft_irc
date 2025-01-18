#pragma once

#include <data/Client.h>
#include <map>

class Command;
class Manager;

class CommandManager 
{
public:
    CommandManager();
    ~CommandManager();

    void executeCommands(Manager& manager, Client& client);
    void executeCommand(Manager& manager, Client& client, const std::string& buffer);

private:
    std::map<std::string, Command*> validCommands;

    CommandManager(const CommandManager& toCopy);
    CommandManager& operator=(const CommandManager& toAssign);
};
