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
    // Not enough parameters.
    if (args.empty())
    {
        Replier::reply(client.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(server.getName(),
            std::string("MODE")));
        return;
    }

    // Fetch channel from server's channel list.
    const std::string& channelName = args[0];
    Channel* channel = server.getChannel(channelName);

    // Channel not found.
    if (channel == NULL)
    {
        Replier::reply(client.getFd(), Replier::errNoSuchChannel, Utils::anyToVec(server.getName(), channelName));
        return;
    }

    // Client not a member of the channel.
    if (!channel->isUserOnChannel(client.getNickname()))
    {
        Replier::reply(client.getFd(), Replier::errNotOnChannel, Utils::anyToVec(server.getName(), channelName));
        return;
    }

    // No operator privileges.
    if (!channel->isUserOperator(client.getNickname()))
    {
        Replier::reply(client.getFd(), Replier::errChanOPrivsNeeded,
            Utils::anyToVec(server.getName(), channelName));
        return;
    }

    // Client requesting to see current channel modes.
    if (args.size() == 1)
    {
        sendCurrentChannelModes(server.getName(), *channel, client.getFd());
        return;
    }

    // Client requesting to modify channel modes.
    editChannelModes(args, client.getFd(), server.getName(), *channel);
}

void Mode::sendCurrentChannelModes(const std::string& serverName, const Channel& channel, const int clientFd) const
{
    const std::string& channelName = channel.getName();
    std::string modes = "+";
    std::vector<std::string> replyParams = Utils::anyToVec(serverName, channelName);

    const std::string& key = channel.getKey();
    if (!key.empty())
    {
        modes += "k";
        replyParams.push_back(key);
    }

    if (channel.isInviteOnly())
    {
        modes += "i";
    }

    if (channel.isTopicRestricted())
    {
        modes += "t";
    }

    if (channel.isUserLimitActive())
    {
        modes += "l";
        replyParams.push_back(Utils::intToString(static_cast<int>(channel.getUserLimit())));
    }

    if (modes.length() > 1)
    {
        replyParams.insert(replyParams.begin() + 2, modes);
    }

    Replier::reply(clientFd, Replier::rplChannelModeIs, replyParams);
}

void Mode::editChannelModes(const std::vector<std::string>& args, const int clientFd, const std::string& serverName,
    Channel& channel) const
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
            channel.setChannelInviteOnly(isInviteOnly);
        }
        // Switch on/off topic change available only for channel operators.
        else if (modes[i] == 't')
        {
            const bool isTopicRestricted = (action == "+");
            channel.setTopicRestricted(isTopicRestricted);
        }
        // Remove/add channel key(password).
        else if (modes[i] == 'k')
        {
            handleKeyMode(channel, action, args, argsI, clientFd, serverName, modes[i]);
        }
        // Give/remove channel operator privilege to/from user.
        else if (modes[i] == 'o')
        {
            handleOperatorMode(channel, action, args, argsI, clientFd, serverName, modes[i]);
        }
        // Enable+set/remove user limit on the channel.
        else if (modes[i] == 'l')
        {
            handleUserLimitMode(channel, action, args, argsI, clientFd, serverName, modes[i]);
        }
    }
}

void Mode::handleKeyMode(Channel& channel, const std::string& action, const std::vector<std::string>& args,
    size_t& argsI, const int clientFd, const std::string& serverName, const char mode) const
{
    if (action == "-")
    {
        channel.setKey(std::string());
        return;
    }

    if (args[argsI].empty())
    {
        Replier::reply(clientFd, Replier::errNeedMoreParams, Utils::anyToVec(serverName, std::string(1, mode)));
        return;
    }

    const std::string& key = channel.getKey();
    if (!key.empty())
    {
        const std::string& channelName = channel.getName();
        Replier::reply(clientFd, Replier::errKeySet, Utils::anyToVec(serverName, channelName));
        return;
    }

    const std::string& newKey = args[argsI];
    ++argsI;
    channel.setKey(newKey);
}

void Mode::handleOperatorMode(Channel& channel, const std::string& action, const std::vector<std::string>& args,
    size_t& argsI, const int clientFd, const std::string& serverName, const char mode) const
{
    if (args[argsI].empty())
    {
        Replier::reply(clientFd, Replier::errNeedMoreParams, Utils::anyToVec(serverName, std::string(1, mode)));
        return;
    }

    const std::string& targetNickname = args[argsI];
    ++argsI;
    if (!channel.isUserOnChannel(targetNickname))
    {
        Replier::reply(clientFd, Replier::errNoSuchNick, Utils::anyToVec(serverName, targetNickname));
        return;
    }

    const bool operatorPrivilege = (action == "+");
    channel.setOperator(targetNickname, operatorPrivilege);
}

void Mode::handleUserLimitMode(Channel& channel, const std::string& action, const std::vector<std::string>& args,
    size_t& argsI, const int clientFd, const std::string& serverName, const char mode) const
{
    if (action == "-")
    {
        channel.disableUserLimit();
        return;
    }

    if (args[argsI].empty())
    {
        Replier::reply(clientFd, Replier::errNeedMoreParams, Utils::anyToVec(serverName, std::string(1, mode)));
        return;
    }

    const int limit = Utils::cStringToPositiveInt(args[argsI].c_str());
    ++argsI;
    channel.setUserLimit(limit);
}