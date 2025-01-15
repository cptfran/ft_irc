#include "commands/Mode.h"
#include "server/Server.h"
#include "utils/Utils.h"
#include "replier/Replier.h"
#include "channel/Channel.h"

Mode::Mode()
{

}

Mode::~Mode()
{

}

void Mode::execute(Server& server, Client& requester, const std::vector<std::string>& args) const
{
    // Not enough parameters.
    if (args.empty())
    {
        Replier::addToQueue(requester.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(server.getName(),
            requester.getNickname(), std::string("MODE")));
        return;
    }

    // Fetch channel from server's channel list.
    const std::string& channelName = args[0];

    // Prompting for user modes. User modes not handled by the server.
    if (!channelName.empty() && channelName[0] != '#')
    {
        handleUserModes(requester, channelName, server, args);
        return;
    }

    Channel* channel = server.getChannel(channelName);

    // Channel not found.
    if (channel == NULL)
    {
        Replier::addToQueue(requester.getFd(), Replier::errNoSuchChannel, Utils::anyToVec(server.getName(),
            requester.getNickname(), channelName));
        return;
    }

    // Client not a member of the channel.
    if (!channel->isUserOnChannel(requester.getNickname()))
    {
        Replier::addToQueue(requester.getFd(), Replier::errNotOnChannel, Utils::anyToVec(server.getName(), channelName));
        return;
    }

    // Client requesting to see current channel modes.
    if (args.size() == 1)
    {
        sendCurrentChannelModes(server.getName(), *channel, requester);
        return;
    }

    // Client requesting for ban list.
    const std::string& modes = args[1];
    if (args.size() == 2 && modes == "b")
    {
        Replier::addToQueue(requester.getFd(), Replier::rplEndOfBanList, Utils::anyToVec(server.getName(),
            requester.getNickname(), channelName));
        return;
    }

    // No operator privileges.
    if (!channel->isUserOperator(requester.getNickname()))
    {
        Replier::addToQueue(requester.getFd(), Replier::errChanOPrivsNeeded, Utils::anyToVec(server.getName(),
            requester.getNickname(), channelName));
        return;
    }

    // Client requesting to modify channel modes.
    editChannelModes(args, requester, server, *channel);
}

void Mode::handleUserModes(Client& requester, const std::string& nickname, const Server& server,
    const std::vector<std::string>& args) const
{
    if (nickname != requester.getNickname())
    {
        Replier::addToQueue(requester.getFd(), Replier::errUsersDontMatch, Utils::anyToVec(server.getName(),
            requester.getNickname()));
        return;
    }

    if (args.size() < 2)
    {
        const std::string modes = requester.isInvisible() ? std::string("+i") : std::string();
        Replier::addToQueue(requester.getFd(), Replier::rplUModeIs, Utils::anyToVec(server.getName(),
            requester.getNickname(), modes));
        return;
    }

    const std::string& modes = args[1];
    std::string action;

    for (size_t i = 0; i < modes.length(); ++i)
    {
        // Check if user requests to add or remove a mode.
        if (modes[i] == '+' || modes[i] == '-')
        {
            action = modes[i];
        }
        // Action sign not found.
        else if (action.empty() || server.getAvailableUserModes().find(modes[i]) == std::string::npos)
        {
            Replier::addToQueue(requester.getFd(), Replier::errUModeUnknownFlag, Utils::anyToVec(server.getName(),
                requester.getNickname()));
        }
        // Switch on/off invisible mode.
        else
        {
            const bool invisible (action == "+");
            requester.setInvisible(invisible);
        }
    }
}

void Mode::sendCurrentChannelModes(const std::string& serverName, const Channel& channel, const Client& requester) const
{
    std::string modes = "+";
    std::vector<std::string> replyParams = Utils::anyToVec(serverName, requester.getNickname(), channel.getName());

    if (!channel.getKey().empty())
    {
        modes += "k";
        replyParams.push_back(channel.getKey());
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
        replyParams.insert(replyParams.begin() + 3, modes);
    }

    Replier::addToQueue(requester.getFd(), Replier::rplChannelModeIs, replyParams);
}

void Mode::editChannelModes(const std::vector<std::string>& args, const Client& requester,
    const Server& server, Channel& channel) const
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
        else if (action.empty() || server.getAvailableChannelModes().find(modes[i]) == std::string::npos)
        {
            Replier::addToQueue(requester.getFd(), Replier::errUnknownMode, Utils::anyToVec(server.getName(),
                requester.getNickname(), std::string(1, modes[i])));
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
            handleKeyMode(channel, action, args, argsI, requester, server.getName(), modes[i]);
        }
        // Give/remove channel operator privilege to/from user.
        else if (modes[i] == 'o')
        {
            handleOperatorMode(channel, action, args, argsI, requester, server.getName(), modes[i]);
        }
        // Enable+set/remove user limit on the channel.
        else if (modes[i] == 'l')
        {
            handleUserLimitMode(channel, action, args, argsI, requester, server.getName(), modes[i]);
        }
    }
}

void Mode::handleKeyMode(Channel& channel, const std::string& action, const std::vector<std::string>& args,
    size_t& argsI, const Client& requester, const std::string& serverName, const char mode) const
{
    if (action == "-")
    {
        channel.setKey(std::string());
        return;
    }

    if (argsI >= args.size())
    {
        Replier::addToQueue(requester.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(serverName,
            requester.getNickname(), std::string(1, mode)));
        return;
    }

    const std::string& key = channel.getKey();
    if (!key.empty())
    {
        const std::string& channelName = channel.getName();
        Replier::addToQueue(requester.getFd(), Replier::errKeySet, Utils::anyToVec(serverName,
            requester.getNickname(), channelName));
        return;
    }

    const std::string& newKey = args[argsI];
    channel.setKey(newKey);
}

void Mode::handleOperatorMode(Channel& channel, const std::string& action, const std::vector<std::string>& args,
    size_t& argsI, const Client& requester, const std::string& serverName, const char mode) const
{
    if (argsI >= args.size())
    {
        Replier::addToQueue(requester.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(serverName,
            requester.getNickname(), std::string(1, mode)));
        return;
    }

    const std::string& targetNickname = args[argsI];
    ++argsI;
    if (!channel.isUserOnChannel(targetNickname))
    {
        Replier::addToQueue(requester.getFd(), Replier::errNoSuchNick, Utils::anyToVec(serverName, targetNickname));
        return;
    }

    const bool operatorPrivilege = (action == "+");
    channel.setOperator(targetNickname, operatorPrivilege);
}

void Mode::handleUserLimitMode(Channel& channel, const std::string& action, const std::vector<std::string>& args,
    size_t& argsI, const Client& requester, const std::string& serverName, const char mode) const
{
    if (action == "-")
    {
        channel.disableUserLimit();
        return;
    }

    if (argsI >= args.size())
    {
        Replier::addToQueue(requester.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(serverName,
            requester.getNickname(), std::string(1, mode)));
        return;
    }

    const int limit = Utils::cStringToPositiveInt(args[argsI].c_str());
    ++argsI;
    channel.setUserLimit(limit);
}