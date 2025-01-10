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
    // Target not provided.
    if (args.empty() || (args.size() >= 1 && args[0][0] == ':'))
    {
        Replier::reply(client.getFd(), Replier::errNoRecipient, Utils::anyToVec(server.getName(),
            client.getNickname(), std::string("PRIVMSG")));
        return;
    }

    // Message not provided.
    if (args.size() < 2 || args[1][0] != ':')
    {
        Replier::reply(client.getFd(), Replier::errNoTextToSend, Utils::anyToVec(server.getName(),
            client.getNickname()));
        return;
    }

    const std::string& targets = args[0];
    std::vector<std::string> extrTargets = ClientTranslator::extractPrivmsgTargets(targets);

    std::vector<Client> matchedTargets;
    for (std::vector<std::string>::iterator it = extrTargets.begin(); it != extrTargets.end(); ++it)
    {
        std::vector<Client> foundTargets = findTargetsOnServer(client, server, *it);
        matchedTargets.insert(matchedTargets.end(), foundTargets.begin(), foundTargets.end());
    }
}

std::vector<Client> Privmsg::findTargetsOnServer(Client& requester, Server& server,
    const std::string& extrTarget) const
{
    std::vector<Client> targets;
    const std::map<int, Client>& clients = server.getClients();

    if (extrTarget[0] == '#')
    {
        const Channel* channel = server.getChannel(extrTarget);
        if (channel != NULL)
        {
            return channel->getClientList();
        }

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
                if (ClientTranslator::matchWildcard(extrTarget.substr(1).c_str(), it->second.getHostname().c_str()))
                {
                    targets.push_back(it->second);
                }
            }
            
            if (targets.empty())
            {
                Replier::reply(requester.getFd(), Replier::errNoSuchNick, Utils::anyToVec(server.getName(),
                    requester.getNickname(), extrTarget));
                return std::vector<Client>();
            }

            return targets;
        }
    }

    if (extrTarget[0] == '$')
    {
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

            // TODO: need to change this to match with the server name.
            for (std::map<int, Client>::const_iterator it = clients.begin(); it != clients.end(); ++it)
            {
                if (ClientTranslator::matchWildcard(extrTarget.substr(1).c_str(), it->second.getHostname().c_str()))
                {
                    targets.push_back(it->second);
                }
            }

            if (targets.empty())
            {
                Replier::reply(requester.getFd(), Replier::errNoSuchNick, Utils::anyToVec(server.getName(),
                    requester.getNickname(), extrTarget));
                return std::vector<Client>();
            }

            return targets;
        }
    }

    std::string nickname;
    std::string username;
    std::string hostOrServerName;

    size_t splitNicknamePos = extrTarget.find('!');
    size_t splitUsernamePos = extrTarget.find('%');

    if (splitNicknamePos != std::string::npos && splitUsernamePos != std::string::npos)
    {
        Replier::reply(requester.getFd(), Replier::errNoSuchNick, Utils::anyToVec(server.getName(),
            requester.getNickname(), extrTarget));
        return std::vector<Client>();
    }

    if (splitNicknamePos != std::string::npos)
    {
        nickname = extrTarget.substr(0, splitNicknamePos);
        hostOrServerName = extrTarget.substr(splitNicknamePos);
    }

    if (splitUsernamePos != std::string::npos)
    {
        username = extrTarget.substr(0, splitUsernamePos);
        hostOrServerName = extrTarget.substr(splitUsernamePos);
    }

    // TODO: delete this method as now Im fetching all client list from server. This can be done here without server
    // method.
    targets = server.findClientsByNickUserHostServerName(nickname, username, hostOrServerName);

    if (targets.size() > 1)
    {
        std::string errorCode = "Ambiguous " + extrTarget + ".";
        Replier::reply(requester.getFd(), Replier::errTooManyTargets, Utils::anyToVec(server.getName(),
            requester.getNickname(), extrTarget, errorCode, std::string("Message not sent.")));
        return std::vector<Client>();
    }

    return targets;
}
