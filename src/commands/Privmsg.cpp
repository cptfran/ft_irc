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
    std::vector<std::string> extractedTargets = ClientTranslator::extractPrivmsgTargets(targets);

    std::vector<Client> sanitizedTargets;
    for (std::vector<std::string>::iterator it = extractedTargets.begin(); it != extractedTargets.end(); ++it)
    {
        std::vector<Client> foundTargets = findTargetsOnServer(client, server, *it);
        sanitizedTargets.insert(sanitizedTargets.end(), foundTargets.begin(), foundTargets.end());
    }
}

std::vector<Client> Privmsg::findTargetsOnServer(Client& requester, Server& server,
    const std::string& extractedTarget) const
{
    std::vector<Client> targets;

    if (extractedTarget[0] == '#')
    {
        const Channel* channel = server.getChannel(extractedTarget);
        if (channel != NULL)
        {
            return channel->getClientList();
        }
        // TODO: handle hostname mask.
    }

    if (extractedTarget[0] == '$')
    {
        // TODO: handle servername mask.
    }

    std::string nickname;
    std::string username;
    std::string hostOrServerName;

    size_t splitNicknamePos = extractedTarget.find('!');
    size_t splitUsernamePos = extractedTarget.find('%');

    if (splitNicknamePos != std::string::npos && splitUsernamePos != std::string::npos)
    {
        Replier::reply(requester.getFd(), Replier::errNoSuchNick, Utils::anyToVec(server.getName(),
            requester.getNickname(), extractedTarget));
        return std::vector<Client>();
    }

    if (splitNicknamePos != std::string::npos)
    {
        nickname = extractedTarget.substr(0, splitNicknamePos);
        hostOrServerName = extractedTarget.substr(splitNicknamePos);
    }

    if (splitUsernamePos != std::string::npos)
    {
        username = extractedTarget.substr(0, splitUsernamePos);
        hostOrServerName = extractedTarget.substr(splitUsernamePos);
    }

    targets = server.findClientsByNickUserHostServerName(nickname, username, hostOrServerName);

    if (targets.size() > 1)
    {
        std::string errorCode = "Ambiguous " + extractedTarget + ".";
        Replier::reply(requester.getFd(), Replier::errTooManyTargets, Utils::anyToVec(server.getName(),
            requester.getNickname(), extractedTarget, errorCode, std::string("Message not sent.")));
        return std::vector<Client>();
    }

    return targets;
}
