#include "commands/Privmsg.h"
#include "server/Server.h"
#include "utils/Utils.h"
#include "replier/Replier.h"
#include "channel/Channel.h"
#include "client/ClientTranslator.h"

#define MAX_PRIVMSG_TARGETS 5

Privmsg::Privmsg()
{

}

Privmsg::~Privmsg()
{

}

void Privmsg::execute(Server& server, Client& requester, const std::vector<std::string>& args) const
{
    // Target not provided.
    if (args.empty() || (args.size() >= 1 && args[0][0] == ':'))
    {
        Replier::addToQueue(requester.getFd(), Replier::errNoRecipient, Utils::anyToVec(server.getName(),
            requester.getNickname(), std::string("PRIVMSG")));
        return;
    }

    // Message not provided.
    if (args.size() < 2 || args[1][0] != ':')
    {
        Replier::addToQueue(requester.getFd(), Replier::errNoTextToSend, Utils::anyToVec(server.getName(),
            requester.getNickname()));
        return;
    }

    // Find all the targets that match with connected clients.
    const std::string& targetsCommaSeparated = args[0];
    std::vector<std::pair<Client, std::string> > targets = findMatchingTargets(requester, server,
        targetsCommaSeparated);

    const std::string& message = ClientTranslator::sanitizeColonMessage(args[1]);
    sendMessagesToTargets(requester, targets, message);
}

std::vector<std::pair<Client, std::string> > Privmsg::findMatchingTargets(const Client& requester, Server& server, 
    const std::string& targets) const
{
    std::vector<std::string> extrTargets = ClientTranslator::splitToTokens(targets, ',');

    std::vector<std::pair<Client, std::string> > foundTargets;
    for (std::vector<std::string>::const_iterator it = extrTargets.begin(); it != extrTargets.end(); ++it)
    {
        std::vector<std::pair<Client, std::string> > partOfFoundTargets = getTargetsFromServer(requester, server, *it);
        if (partOfFoundTargets.empty())
        {
            return std::vector<std::pair<Client, std::string> >();
        }
        foundTargets.insert(foundTargets.end(), partOfFoundTargets.begin(), partOfFoundTargets.end());
    }

    return foundTargets;
}

void Privmsg::sendMessagesToTargets(const Client& client, const std::vector<std::pair<Client, std::string> >& targets, 
    const std::string& message) const
{
    for (std::vector<std::pair<Client, std::string> >::const_iterator it = targets.begin(); it != targets.end(); ++it)
    {
        Replier::addToQueue(it->first.getFd(), Replier::rplPrivmsg, Utils::anyToVec(client.getNickname(), 
            client.getUsername(), client.getHostname(), it->second, message));
    }
}

std::vector<std::pair<Client, std::string> > Privmsg::getTargetsFromServer(const Client& requester, Server& server, 
    const std::string& extrTarget) const
{
    if (extrTarget[0] == '#')
    {
        std::vector<std::pair<Client, std::string> > targets = getChannelTargets(requester, server, extrTarget);
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

std::vector<std::pair<Client, std::string> > Privmsg::getChannelTargets(const Client& requester, Server& server, 
    const std::string& extrTarget) const
{
    const Channel* channel = server.getChannel(extrTarget);

    if (channel == NULL)
    {
        return std::vector<std::pair<Client, std::string> >();
    }

    if (!channel->isUserOnChannel(requester.getNickname()))
    {
        Replier::addToQueue(requester.getFd(), Replier::errCannotSendToChan, Utils::anyToVec(server.getName(),
            requester.getNickname(), channel->getName()));
        return std::vector<std::pair<Client, std::string> >();
    }

    const std::vector<Client>& clientList = channel->getClientList();
    std::vector<std::pair<Client, std::string> > channelTargets;
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

std::vector<std::pair<Client, std::string> > Privmsg::getHostnameTargetsByWildcard(const Client& requester, 
    const Server& server, const std::string& extrTarget) const
{
    std::vector<std::pair<Client, std::string> > targets;
    const std::map<int, Client>& clients = server.getClients();

    size_t dotPos = extrTarget.find('.');
    if (dotPos != std::string::npos)
    {
        const std::string afterDot = extrTarget.substr(dotPos);
        if (afterDot.empty() || afterDot.find('*') != std::string::npos)
        {
            Replier::addToQueue(requester.getFd(), Replier::errNoSuchNick, Utils::anyToVec(server.getName(),
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
        Replier::addToQueue(requester.getFd(), Replier::errNoSuchNick, Utils::anyToVec(server.getName(),
            requester.getNickname(), extrTarget));
    }
    
    return targets;
}

std::vector<std::pair<Client, std::string> > Privmsg::getServerTargetsByWildcard(const Client& requester, 
    const Server& server, const std::string& extrTarget) const
{
    std::vector<std::pair<Client, std::string> > targets;

    size_t dotPos = extrTarget.find('.');
    if (dotPos != std::string::npos)
    {
        const std::string afterDot = extrTarget.substr(dotPos + 1);
        if (afterDot.empty() || afterDot.find('*') != std::string::npos)
        {
            Replier::addToQueue(requester.getFd(), Replier::errNoSuchNick, Utils::anyToVec(server.getName(),
                requester.getNickname(), extrTarget));
            return std::vector<std::pair<Client, std::string> >();
        }

        const std::map<int, Client>& clients = server.getClients();
        if (ClientTranslator::matchWildcard(extrTarget.c_str(), server.getName().c_str()))
        {
            for (std::map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it)
            {
                targets.push_back(std::make_pair(it->second, server.getName()));
            }
        }
    }
    if (targets.empty())
    {
        Replier::addToQueue(requester.getFd(), Replier::errNoSuchNick, Utils::anyToVec(server.getName(),
            requester.getNickname(), extrTarget));
    }

    return targets;
}

std::vector<std::pair<Client, std::string> > Privmsg::getUserTargets(const Client& requester, const Server& server,
    const std::string& extrTarget) const
{
    std::string nickname, username, hostname, serverName;
    ClientTranslator::parseUserTarget(extrTarget, nickname, username, hostname, serverName);

    std::vector<std::pair<Client, std::string> > targets;
    const std::map<int, Client>& clients = server.getClients();

    for (std::map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it)
    {
        const bool allEmpty = (hostname.empty() && serverName.empty() && nickname.empty() && username.empty());
        const bool nickMatches = (nickname.empty() || nickname == it->second.getNickname());
        const bool userMatches = (username.empty() || username == it->second.getUsername());
        const bool hostMatches = (hostname.empty() || hostname == it->second.getHostname());
        const bool serverMatches = (serverName.empty() || serverName == server.getName());
        const bool hostServerMatches = ((hostMatches && serverMatches) ||
                                  ((hostMatches || serverMatches) && hostname == serverName));

        if (!allEmpty && nickMatches && userMatches && hostServerMatches)
        {
            targets.push_back(std::make_pair(it->second, extrTarget));
        }
    }

    if (targets.size() > 1)
    {
        std::string errorCode = "Ambiguous " + extrTarget + ".";
        Replier::addToQueue(requester.getFd(), Replier::errTooManyTargets, Utils::anyToVec(server.getName(),
            requester.getNickname(), extrTarget, errorCode, std::string("Message not sent.")));
        return std::vector<std::pair<Client, std::string> >();
    }

    if (targets.empty())
    {
        Replier::addToQueue(requester.getFd(), Replier::errNoSuchNick, Utils::anyToVec(server.getName(),
            requester.getNickname(), extrTarget));
    }

    return targets;
}