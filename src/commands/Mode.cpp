#include "commands/Mode.h"
#include "Log.h"
#include "Server.h"
#include "Utils.h"
#include "Replier.h"

Mode::Mode()
{

}

Mode::~Mode()
{

}

//          ERR_NEEDMOREPARAMS (V)          RPL_CHANNELMODEIS (V)
//          ERR_CHANOPRIVSNEEDED (V)        ERR_NOSUCHNICK (V)
//          ERR_NOTONCHANNEL (V)            ERR_KEYSET (V)
//          ERR_UNKNOWNMODE (V)             ERR_NOSUCHCHANNEL (V)

// TODO: some of the code is repeating in few execute methods, maybe can be done better. (like another mutual methods)
// TODO: maybe rebuild getClientData as I don't need to fetch everything?
// TODO: make funcs for checks that return numeric reply and then reply based on the return?
void Mode::execute(Server& server, Client& client, const std::vector<std::string>& args) const
{
    const int clientFd = client.getFd();
    const std::string& serverName = server.getName();

    // Not enough parameters.
    if (args.empty())
    {
        const std::string command = "MODE";
        Replier::reply(clientFd, Replier::errNeedMoreParams, Utils::anyToVec(serverName, command));
        return;
    }

    const std::string& channelName = args[0];
    Channel* channel;

    // Fetch channel from server's channel list.
    try
    {
        channel = &server.findChannel(channelName);
    }
    // Channel not found.
    catch (const std::exception&)
    {
        Replier::reply(clientFd, Replier::errNoSuchChannel, Utils::anyToVec(serverName, channelName));
        return;
    }

    // Client not a member of the channel.
    const std::string& requestorNickname = client.getNickname();
    if (!channel->isClientOnChannel(requestorNickname))
    {
        Replier::reply(clientFd, Replier::errNotOnChannel, Utils::anyToVec(serverName, channelName));
        return;
    }

    // No operator privileges.
    if (!channel->isClientOperator(client))
    {
        Replier::reply(clientFd, Replier::errChanOPrivsNeeded, Utils::anyToVec(serverName, channelName));
        return;
    }

    // Client prompting for current modes.
    if (args.size() == 1)
    {
        std::string modes = "+";
        std::vector<std::string> replyParams = Utils::anyToVec(serverName, channelName);

        const std::string& key = channel->getKey();
        if (!key.empty())
        {
            modes += "k";
            replyParams.push_back(key);
        }

        if (channel->isInviteOnly())
        {
            modes += "i";
        }

        if (channel->isTopicRestricted())
        {
            modes += "t";
        }

        if (channel->isUserLimitActive())
        {
            const int userLimit = channel->getUserLimit();
            modes += "l";
            replyParams.push_back(Utils::intToString(userLimit));
        }

        if (modes.length() > 1)
        {
            replyParams.insert(replyParams.begin() + 2, modes);
        }

        Replier::reply(clientFd, Replier::rplChannelModeIs, replyParams);
        return;
    }

    const std::string& modes = args[1];
    std::string action;
    size_t argsI = 2;
    for (size_t i = 0; i < modes.length(); ++i)
    {
        if (modes[i] == '+' || modes[i] == '-')
        {
            action = modes[i];
        }
        else if (action.empty())
        {
            Replier::reply(clientFd, Replier::errUnknownMode,
                Utils::anyToVec(serverName, std::string(1, modes[i])));
        }
        else if (modes[i] == 'i')
        {
            bool isInviteOnly (action == "+");
            channel->setChannelInviteOnly(isInviteOnly);
        }
        else if (modes[i] == 't')
        {
            bool isTopicRestricted = (action == "+");
            channel->setTopicRestricted(isTopicRestricted);
        }
        else if (modes[i] == 'k')
        {
            if (action == "-")
            {
                channel->setKey(std::string());
                continue;
            }
            if (args[argsI].empty())
            {
                Replier::reply(clientFd, Replier::errNeedMoreParams,
                    Utils::anyToVec(serverName, std::string(1, modes[i])));
                continue;
            }
            const std::string& key = channel->getKey();
            if (!key.empty())
            {
                Replier::reply(clientFd, Replier::errKeySet, Utils::anyToVec(serverName, channelName));
                continue;
            }
            const std::string& newKey = args[argsI];
            ++argsI;
            channel->setKey(newKey);
        }
        else if (modes[i] == 'o')
        {
            if (args[argsI].empty())
            {
                Replier::reply(clientFd, Replier::errNeedMoreParams,
                    Utils::anyToVec(serverName, std::string(1, modes[i])));
                continue;
            }
            const std::string& targetNickname = args[argsI];
            ++argsI;
            if (!channel->isClientOnChannel(targetNickname))
            {
                Replier::reply(clientFd, Replier::errNoSuchNick, Utils::anyToVec(serverName, targetNickname));
                continue;
            }
            bool operatorPrivilege = (action == "+");
            channel->setOperator(targetNickname, operatorPrivilege);
        }
        else if (modes[i] == 'l')
        {
            if (action == "-")
            {
                channel->disableUserLimit();
            }
            if (args[argsI].empty())
            {
                Replier::reply(clientFd, Replier::errNeedMoreParams,
                    Utils::anyToVec(serverName, std::string(1, modes[i])));
                continue;
            }
            const int limit = std::stoi(args[argsI]);
            channel->setUserLimit(limit);
        }
    }
}