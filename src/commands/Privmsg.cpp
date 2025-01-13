#include "commands/Privmsg.h"
#include "Server.h"
#include "Utils.h"
#include "Replier.h"
#include "Channel.h"
#include "ClientTranslator.h"

#define MAX_PRIVMSG_TARGETS 5

Privmsg::Privmsg()
{

}

Privmsg::~Privmsg()
{

}

void Privmsg::execute(Server& server, Client& client, const std::vector<std::string>& args) const
{
    if (!validateArgs(server, client, args))
    {
        return;
    }

    const std::string& targetsCommaSeparated = args[0];
    std::vector<Client> foundTargets = findMatchingTargets(client, server, targetsCommaSeparated);
    const std::string& message = ClientTranslator::sanitizeColonMessage(args[1]);
    sendMessagesToTargets(client, foundTargets, message);
}

bool Privmsg::validateArgs(Server& server, Client& client, const std::vector<std::string>& args) const
{
    if (args.empty() || (args.size() >= 1 && args[0][0] == ':'))
    {
        Replier::reply(client.getFd(), Replier::errNoRecipient, Utils::anyToVec(server.getName(), client.getNickname(),
            std::string("PRIVMSG")));
        return false;
    }

    if (args.size() < 2 || args[1][0] != ':')
    {
        Replier::reply(client.getFd(), Replier::errNoTextToSend, Utils::anyToVec(server.getName(), client.getNickname()));
        return false;
    }

    return true;
}

std::vector<Client> Privmsg::findMatchingTargets(Client& client, Server& server, const std::string& targets) const
{
    std::vector<std::string> extrTargets = ClientTranslator::splitToTokens(targets, ',');

    std::vector<Client> foundTargets;
    for (std::vector<std::string>::const_iterator it = extrTargets.begin(); it != extrTargets.end(); ++it)
    {
        // TODO: check here if partOfFoundTargets empty? because then probably cannot execute and return error?
        std::vector<Client> partOfFoundTargets = getTargetsFromServer(client, server, *it);
        foundTargets.insert(foundTargets.end(), partOfFoundTargets.begin(), partOfFoundTargets.end());
    }

    return foundTargets;
}

void Privmsg::sendMessagesToTargets(const Client& client, const std::vector<Client>& foundTargets, 
    const std::string& message) const
{
    for (std::vector<Client>::const_iterator it = foundTargets.begin(); it != foundTargets.end(); ++it)
    {
        Replier::reply(it->getFd(), Replier::rplPrivmsg, Utils::anyToVec(client.getNickname(), client.getUsername(),
            client.getHostname(), it->getNickname(), message));
    }
}

std::vector<Client> Privmsg::getTargetsFromServer(Client& requester, Server& server, const std::string& extrTarget) 
    const
{
    if (extrTarget[0] == '#')
    {
        std::vector<Client> targets = getChannelTargets(server, extrTarget);
        if (!targets.empty())
        {
            return targets;
        }

        return getHostnameTargetsByWildcard(requester, server, extrTarget.substr(1));
    }
    if (extrTarget[0] == '$')
    {
        return getServerTargetsByWildcard(requester, server, extrTarget.substr(1));
    }

    return getUserTargets(requester, server, extrTarget);
}

std::vector<Client> Privmsg::getChannelTargets(Server& server, const std::string& extrTarget) const
{
    const Channel* channel = server.getChannel(extrTarget);
    if (channel != NULL)
    {
        return channel->getClientList();
    }

    return std::vector<Client>();
}

std::vector<Client> Privmsg::getHostnameTargetsByWildcard(Client& requester, Server& server,
    const std::string& extrTarget) const
{
    std::vector<Client> targets;
    const std::map<int, Client>& clients = server.getClients();

    size_t dotPos = extrTarget.find('.');
    if (dotPos != std::string::npos)
    {
        const std::string afterDot = extrTarget.substr(dotPos);
        if (afterDot.empty() || afterDot.find('*') != std::string::npos)
        {
            Replier::reply(requester.getFd(), Replier::errNoSuchNick, Utils::anyToVec(server.getName(),
                requester.getNickname(), extrTarget));
            return std::vector<Client>();
        }

        for (std::map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it)
        {
            if (ClientTranslator::matchWildcard(extrTarget.c_str(), it->second.getHostname().c_str()))
            {
                targets.push_back(it->second);
            }
            return targets;
        }
    }

    Replier::reply(requester.getFd(), Replier::errNoSuchNick, Utils::anyToVec(server.getName(),
        requester.getNickname(), extrTarget));
    return std::vector<Client>();
}

std::vector<Client> Privmsg::getServerTargetsByWildcard(Client& requester, Server& server,
    const std::string& extrTarget) const
{
    size_t dotPos = extrTarget.find('.');
    if (dotPos != std::string::npos)
    {
        const std::string afterDot = extrTarget.substr(dotPos + 1);
        if (afterDot.empty() || afterDot.find('*') != std::string::npos)
        {
            Replier::reply(requester.getFd(), Replier::errNoSuchNick, Utils::anyToVec(server.getName(),
                requester.getNickname(), extrTarget));
            return std::vector<Client>();
        }

        std::vector<Client> targets;
        const std::map<int, Client>& clients = server.getClients();
        if (ClientTranslator::matchWildcard(extrTarget.c_str(), server.getName().c_str()))
        {
            for (std::map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it)
            {
                targets.push_back(it->second);
            }
            return targets;
        }
    }

    Replier::reply(requester.getFd(), Replier::errNoSuchNick, Utils::anyToVec(server.getName(),
        requester.getNickname(), extrTarget));
    return std::vector<Client>();
}

std::vector<Client> Privmsg::getUserTargets(Client& requester, Server& server, const std::string& extrTarget) const
{
    std::string nickname, username, hostname, serverName;
    parseUserTarget(extrTarget, nickname, username, hostname, serverName);

    std::vector<Client> targets;
    const std::map<int, Client>& clients = server.getClients();

    for (std::map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it)
    {
        bool allEmpty = (hostname.empty() && serverName.empty() && nickname.empty() && username.empty());
        bool hostMatches = (hostname.empty() || hostname == it->second.getHostname());
        bool serverMatches = (serverName.empty() || serverName == server.getName());
        bool nickMatches = (nickname.empty() || nickname == it->second.getNickname());
        bool userMatches = (username.empty() || username == it->second.getUsername());

        if (!allEmpty && hostMatches && serverMatches && nickMatches && userMatches)
        {
            targets.push_back(it->second);
        }
    }

    if (targets.size() > 1)
    {
        std::string errorCode = "Ambiguous " + extrTarget + ".";
        Replier::reply(requester.getFd(), Replier::errTooManyTargets, Utils::anyToVec(server.getName(),
            requester.getNickname(), extrTarget, errorCode, std::string("Message not sent.")));
        return std::vector<Client>();
    }

    return targets;
}

void Privmsg::parseUserTarget(const std::string& extrTarget, std::string& nickname, std::string& username, 
    std::string& hostname, std::string& serverName) const
{
    size_t splitPos = extrTarget.find('!');
    if (splitPos != std::string::npos)
    {
        nickname = extrTarget.substr(0, splitPos);
        username = extrTarget.substr(splitPos + 1);

        splitPos = username.find('%');
        if (splitPos != std::string::npos)
        {
            hostname = username.substr(splitPos + 1);
            username = username.substr(0, splitPos);

            splitPos = username.find('@');
            if (splitPos != std::string::npos)
            {
                serverName = hostname.substr(splitPos + 1);
                hostname = hostname.substr(0, splitPos);
            }
        }
        else
        {
            splitPos = username.find('@');
            if (splitPos != std::string::npos)
            {
                hostname, serverName = username.substr(splitPos + 1);
                username = username.substr(0, splitPos);
            }
        }
    }
    else
    {
        splitPos = extrTarget.find('%');
        if (splitPos != std::string::npos)
        {
            username = extrTarget.substr(0, splitPos);
            hostname = extrTarget.substr(splitPos + 1);

            splitPos = hostname.find('@');
            if (splitPos != std::string::npos)
            {
                serverName = hostname.substr(splitPos + 1);
                hostname = hostname.substr(0, splitPos);
            }
        }
        else
        {
            splitPos = extrTarget.find('@');
            if (splitPos != std::string::npos)
            {
                username = extrTarget.substr(0, splitPos);
                hostname, serverName = extrTarget.size(splitPos + 1);
            }
            else
            {
                nickname = extrTarget;
            }
        }
    }
}