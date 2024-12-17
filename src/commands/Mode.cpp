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
        channel = &server.getChannel(channelName);
    }
    // Channel not found.
    catch (const std::exception&)
    {
        Replier::reply(clientFd, Replier::errNoSuchChannel, Utils::anyToVec(serverName, channelName));
        return;
    }

    // Client not a member of the channel.
    const std::string& requestorNickname = client.getNickname();
    if (!channel->isUserOnChannel(requestorNickname))
    {
        Replier::reply(clientFd, Replier::errNotOnChannel, Utils::anyToVec(serverName, channelName));
        return;
    }

    // No operator privileges.
    if (!channel->isUserOperator(requestorNickname))
    {
        Replier::reply(clientFd, Replier::errChanOPrivsNeeded, Utils::anyToVec(serverName, channelName));
        return;
    }

    // Client requesting to see current channel modes.
    if (sendCurrentChannelModes(args.size(), serverName, channel, clientFd))
    {
       return;
    }

    // Client requesting to modify channel modes.
    editChannelModes(args, clientFd, serverName, channel);
}

bool Mode::sendCurrentChannelModes(const size_t argsSize, const std::string& serverName, const Channel* channel,
    const int clientFd) const
{
    if (argsSize == 1)
    {
        const std::string& channelName = channel->getName();
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
        return true;
    }

    return false;
}

void Mode::editChannelModes(const std::vector<std::string>& args, const int clientFd,
    const std::string& serverName, Channel* channel) const
{
    const std::string& modes = args[1];
    std::string action;
    size_t argsI = 2;

    for (size_t i = 0; i < modes.length(); ++i)
    {
        // Check if user requests to add or remove a mode.
        if (modes[i] == '+' || modes[i] == '-')
        {
            action = modes[i];
        }

        // Action sign not found.
        else if (action.empty())
        {
            Replier::reply(clientFd, Replier::errUnknownMode,
                Utils::anyToVec(serverName, std::string(1, modes[i])));
        }

        // Switch on/off invite-only mode.
        else if (modes[i] == 'i')
        {
            const bool isInviteOnly (action == "+");
            channel->setChannelInviteOnly(isInviteOnly);
        }

        // Switch on/off topic change available only for channel operators.
        else if (modes[i] == 't')
        {
            const bool isTopicRestricted = (action == "+");
            channel->setTopicRestricted(isTopicRestricted);
        }

        // Remove/add channel key(password).
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
                const std::string& channelName = channel->getName();
                Replier::reply(clientFd, Replier::errKeySet, Utils::anyToVec(serverName, channelName));
                continue;
            }
            const std::string& newKey = args[argsI];
            ++argsI;
            channel->setKey(newKey);
        }

        // Give/remove channel operator privilege to/from user.
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
            if (!channel->isUserOnChannel(targetNickname))
            {
                Replier::reply(clientFd, Replier::errNoSuchNick, Utils::anyToVec(serverName, targetNickname));
                continue;
            }
            const bool operatorPrivilege = (action == "+");
            channel->setOperator(targetNickname, operatorPrivilege);
        }

        // Enable+set/remove user limit on the channel.
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
            const int limit = Utils::cStringToPositiveInt(args[argsI].c_str());
            channel->setUserLimit(limit);
        }
    }
}