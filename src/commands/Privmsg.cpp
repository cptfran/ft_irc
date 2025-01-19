#include "commands/Privmsg.h"
#include "core/Server.h"
#include "utils/Utils.h"
#include "communication/Replier.h"
#include "data/Channel.h"
#include "communication/ClientTranslator.h"

Privmsg::Privmsg() : Command()
{

}

Privmsg::~Privmsg()
{

}

/**
* @brief Executes the PRIVMSG command.
* 
* The execute method is responsible for handling the PRIVMSG command, which is used to send private messages to 
* one or more recipients. This method ensures that the command is properly formatted and that the recipients are 
* valid before sending the message.

* @param serverManager Reference to the server manager.
* @param requester Reference to the client who sent the command.
* @param args Vector of arguments passed with the command.
*/
void Privmsg::execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const
{
    ConfigManager& configManager = serverManager.getConfigManager();

    // Target not provided.
    if (args.empty() || (args.size() >= 1 && args[0][0] == ':'))
    {
        Replier::addToQueue(requester.getFd(), Replier::errNoRecipient, Utils::anyToVec(configManager.getName(),
            requester.getNickname(), std::string("PRIVMSG")));
        return;
    }

    // Message not provided.
    if (args.size() < 2 || args[1][0] != ':')
    {
        Replier::addToQueue(requester.getFd(), Replier::errNoTextToSend, Utils::anyToVec(configManager.getName(),
            requester.getNickname()));
        return;
    }

    // Find all the targets that match with connected clients.
    const std::string& targetsCommaSeparated = args[0];
    const std::vector<std::pair<Client, std::string> > targets = findMatchingTargets(requester, serverManager,
        targetsCommaSeparated);

    const std::string& message = ClientTranslator::sanitizeColonMessage(args[1]);

    for (std::vector<std::pair<Client, std::string> >::const_iterator it = targets.begin(); it != targets.end();
        ++it)
    {
        Replier::addToQueue(it->first.getFd(), Replier::rplPrivmsg, Utils::anyToVec(requester.getNickname(),
            requester.getUsername(), requester.getHostname(), it->second, message));
    }
}

/**
* @brief Finds all matching targets for the PRIVMSG command.
* 
* @param requester Reference to the client who sent the command.
* @param serverManager Reference to the server manager.
* @param targets Comma-separated string of target names.
* @return Vector of pairs containing matching clients and their corresponding target names.
*/
std::vector<std::pair<Client, std::string> > Privmsg::findMatchingTargets(const Client& requester, 
Manager& serverManager, const std::string& targets) const
{
    std::vector<std::string> extrTargets = ClientTranslator::splitToTokens(targets, ',');

    if (extrTargets.size() > ConfigManager::MAX_PRIVMSG_TARGETS)
    {
        Replier::addToQueue(requester.getFd(), Replier::errTooManyTargets, 
            Utils::anyToVec(serverManager.getConfigManager().getName(), requester.getNickname(),
                extrTargets[extrTargets.size() - 1], std::string("Too many targets."), 
                std::string("Message not sent.")));
        return std::vector<std::pair<Client, std::string> >();
    }

    std::vector<std::pair<Client, std::string> > foundTargets;
    for (std::vector<std::string>::const_iterator it = extrTargets.begin(); it != extrTargets.end(); ++it)
    {
        std::vector<std::pair<Client, std::string> > partOfFoundTargets = getTargetsFromServer(requester,
            serverManager, *it);
        if (partOfFoundTargets.empty())
        {
            return std::vector<std::pair<Client, std::string> >();
        }
        foundTargets.insert(foundTargets.end(), partOfFoundTargets.begin(), partOfFoundTargets.end());
    }

    return foundTargets;
}

/**
* @brief Retrieves targets from the server based on the target string.
* 
* @param requester Reference to the client who sent the command.
* @param serverManager Reference to the server manager.
* @param extrTarget Target string to search for.
* @return Vector of pairs containing matching clients and their corresponding target names.
*/
std::vector<std::pair<Client, std::string> > Privmsg::getTargetsFromServer(const Client& requester,
Manager& serverManager, const std::string& extrTarget) const
{
    if (extrTarget[0] == '#')
    {
        std::vector<std::pair<Client, std::string> > targets = getChannelTargets(requester, serverManager, extrTarget);
        if (!targets.empty())
        {
            if (targets.size() == 1 && targets[0].second.empty())
            {
                return std::vector<std::pair<Client, std::string> >();
            }
            return targets;
        }

        return getHostnameTargetsByWildcard(requester, serverManager, extrTarget.substr(1));
    }
    if (extrTarget[0] == '$')
    {
        return getServerTargetsByWildcard(requester, serverManager, extrTarget.substr(1));
    }

    return getUserTargets(requester, serverManager, extrTarget);
}

/**
* @brief Retrieves channel targets for the PRIVMSG command.
* 
* @param requester Reference to the client who sent the command.
* @param serverManager Reference to the server manager.
* @param extrTarget Channel name to search for.
* @return Vector of pairs containing matching clients and their corresponding channel names.
*/
std::vector<std::pair<Client, std::string> > Privmsg::getChannelTargets(const Client& requester, 
Manager& serverManager, const std::string& extrTarget) const
{
    const Channel* channel = serverManager.getChannelManager().getChannel(extrTarget);

    if (channel == NULL)
    {
        return std::vector<std::pair<Client, std::string> >();
    }

    std::vector<std::pair<Client, std::string> > channelTargets;

    if (!channel->isUserOnChannel(requester.getNickname()))
    {
        Replier::addToQueue(requester.getFd(), Replier::errCannotSendToChan, 
            Utils::anyToVec(serverManager.getConfigManager().getName(), requester.getNickname(), channel->getName()));
        channelTargets.push_back(std::make_pair(requester, std::string()));
        return channelTargets;
    }

    const std::vector<Client>& clientList = channel->getClientList();
    for (std::vector<Client>::const_iterator it = clientList.begin(); it != clientList.end(); ++it)
    {
        if (it->getNickname() == requester.getNickname())
        {
            continue;
        }
        channelTargets.push_back(std::make_pair(*it, channel->getName()));
    }

    return channelTargets;
}

/**
* @brief Retrieves hostname targets using wildcard matching.
* 
* @param requester Reference to the client who sent the command.
* @param serverManager Reference to the server manager.
* @param extrTarget Hostname string with wildcard to search for.
* @return Vector of pairs containing matching clients and their corresponding hostnames.
*/
std::vector<std::pair<Client, std::string> > Privmsg::getHostnameTargetsByWildcard(const Client& requester, 
    Manager& serverManager, const std::string& extrTarget) const
{
    std::vector<std::pair<Client, std::string> > targets;
    const std::map<int, Client>& clients = serverManager.getClientManager().getClients();

    ConfigManager& configManager = serverManager.getConfigManager();

    size_t dotPos = extrTarget.find('.');
    if (dotPos != std::string::npos)
    {
        const std::string afterDot = extrTarget.substr(dotPos);
        if (afterDot.empty() || afterDot.find('*') != std::string::npos)
        {
            Replier::addToQueue(requester.getFd(), Replier::errNoSuchNick, Utils::anyToVec(configManager.getName(),
                requester.getNickname(), extrTarget));
            return std::vector<std::pair<Client, std::string> >();
        }

        for (std::map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it)
        {
            if (ClientTranslator::matchWildcard(extrTarget.c_str(), it->second.getHostname().c_str()))
            {
                targets.push_back(std::make_pair(it->second, it->second.getHostname()));
            }
        }
    }

    if (targets.empty())
    {
        Replier::addToQueue(requester.getFd(), Replier::errNoSuchNick, Utils::anyToVec(configManager.getName(),
            requester.getNickname(), extrTarget));
    }
        
    return targets;
}

/**
* @brief Retrieves server targets using wildcard matching.
* 
* @param requester Reference to the client who sent the command.
* @param serverManager Reference to the server manager.
* @param extrTarget Server name string with wildcard to search for.
* @return Vector of pairs containing matching clients and their corresponding server names.
*/
std::vector<std::pair<Client, std::string> > Privmsg::getServerTargetsByWildcard(const Client& requester, 
    Manager& serverManager, const std::string& extrTarget) const
{
    std::vector<std::pair<Client, std::string> > targets;

    ConfigManager& configManager = serverManager.getConfigManager();

    size_t dotPos = extrTarget.find('.');
    if (dotPos != std::string::npos)
    {
        const std::string afterDot = extrTarget.substr(dotPos + 1);
        if (afterDot.empty() || afterDot.find('*') != std::string::npos)
        {
            Replier::addToQueue(requester.getFd(), Replier::errNoSuchNick, Utils::anyToVec(configManager.getName(),
                requester.getNickname(), extrTarget));
            return std::vector<std::pair<Client, std::string> >();
        }

        const std::map<int, Client>& clients = serverManager.getClientManager().getClients();
        if (ClientTranslator::matchWildcard(extrTarget.c_str(), configManager.getName().c_str()))
        {
            for (std::map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it)
            {
                targets.push_back(std::make_pair(it->second, configManager.getName()));
            }
        }
    }
    if (targets.empty())
    {
        Replier::addToQueue(requester.getFd(), Replier::errNoSuchNick, Utils::anyToVec(configManager.getName(),
            requester.getNickname(), extrTarget));
    }

    return targets;
}

/**
* @brief Retrieves user targets for the PRIVMSG command.
* 
* @param requester Reference to the client who sent the command.
* @param serverManager Reference to the server manager.
* @param extrTarget User target string to search for.
* @return Vector of pairs containing matching clients and their corresponding target names.
*/
std::vector<std::pair<Client, std::string> > Privmsg::getUserTargets(const Client& requester, Manager& serverManager,
    const std::string& extrTarget) const
{
    std::string nickname, username, hostname, serverName;
    ClientTranslator::parseUserTarget(extrTarget, nickname, username, hostname, serverName);

    std::vector<std::pair<Client, std::string> > targets;
    const std::map<int, Client>& clients = serverManager.getClientManager().getClients();

    ConfigManager& configManager = serverManager.getConfigManager();

    for (std::map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it)
    {
        bool allEmpty = (hostname.empty() && serverName.empty() && nickname.empty() && username.empty());
        bool nickMatches = (nickname.empty() || nickname == it->second.getNickname());
        bool userMatches = (username.empty() || username == it->second.getUsername());
        bool hostMatches = (hostname.empty() || hostname == it->second.getHostname());
        bool serverMatches = (serverName.empty() || serverName == configManager.getName());
        bool hostServerMatches = ((hostMatches && serverMatches) ||
                                    ((hostMatches || serverMatches) && hostname == serverName));

        if (!allEmpty && nickMatches && userMatches && hostServerMatches)
        {
            targets.push_back(std::make_pair(it->second, extrTarget));
        }
    }

    if (targets.size() > 1)
    {
        std::string errorCode = "Ambiguous " + extrTarget + ".";
        Replier::addToQueue(requester.getFd(), Replier::errTooManyTargets, Utils::anyToVec(configManager.getName(),
            requester.getNickname(), extrTarget, errorCode, std::string("Message not sent.")));
        return std::vector<std::pair<Client, std::string> >();
    }

    if (targets.empty())
    {
        Replier::addToQueue(requester.getFd(), Replier::errNoSuchNick, Utils::anyToVec(configManager.getName(),
            requester.getNickname(), extrTarget));
    }

    return targets;
}