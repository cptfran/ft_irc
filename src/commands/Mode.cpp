#include "commands/Mode.h"
#include "Server.h"
#include "Utils.h"
#include "Replier.h"
#include "Channel.h"

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
            client.getNickname(), std::string("MODE")));
        return;
    }

    // Fetch channel from server's channel list.
    const std::string& channelName = args[0];

    // Prompting for user modes. User modes not handled by the server.
    if (!channelName.empty() && channelName[0] != '#')
    {
        handleUserModes(client, channelName, server, args);
        return;
    }

    Channel* channel = server.getChannel(channelName);

    // Channel not found.
    if (channel == NULL)
    {
        Replier::reply(client.getFd(), Replier::errNoSuchChannel, Utils::anyToVec(server.getName(),
            client.getNickname(), channelName));
        return;
    }

    // Client not a member of the channel.
    if (!channel->isUserOnChannel(client.getNickname()))
    {
        Replier::reply(client.getFd(), Replier::errNotOnChannel, Utils::anyToVec(server.getName(), channelName));
        return;
    }

    // Client requesting to see current channel modes.
    if (args.size() == 1)
    {
        sendCurrentChannelModes(server.getName(), *channel, client);
        return;
    }

    // Client requesting for ban list.
    const std::string& modes = args[1];
    if (args.size() == 2 && modes == "b")
    {
        Replier::reply(client.getFd(), Replier::rplEndOfBanList, Utils::anyToVec(server.getName(),
            client.getNickname(), channelName));
        return;
    }

    // No operator privileges.
    if (!channel->isUserOperator(client.getNickname()))
    {
        Replier::reply(client.getFd(), Replier::errChanOPrivsNeeded, Utils::anyToVec(server.getName(),
            client.getNickname(), channelName));
        return;
    }

    // Client requesting to modify channel modes.
    editChannelModes(args, client, server, *channel);
}

void Mode::handleUserModes(Client& requestor, const std::string& nickname, const Server& server,
    const std::vector<std::string>& args) const
{
    if (nickname != requestor.getNickname())
    {
        Replier::reply(requestor.getFd(), Replier::errUsersDontMatch, Utils::anyToVec(server.getName(),
            requestor.getNickname()));
        return;
    }

    if (args.size() < 2)
    {
        const std::string modes = requestor.isInvisible() ? std::string("+i") : std::string();
        Replier::reply(requestor.getFd(), Replier::rplUModeIs, Utils::anyToVec(server.getName(),
            requestor.getNickname(), modes));
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
            Replier::reply(requestor.getFd(), Replier::errUModeUnknownFlag, Utils::anyToVec(server.getName(),
                requestor.getNickname()));
        }
        // Switch on/off invisible mode.
        else
        {
            const bool invisible (action == "+");
            requestor.setInvisible(invisible);
        }
    }
}

void Mode::sendCurrentChannelModes(const std::string& serverName, const Channel& channel, const Client& requestor) const
{
    std::string modes = "+";
    std::vector<std::string> replyParams = Utils::anyToVec(serverName, requestor.getNickname(), channel.getName());

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

    Replier::reply(requestor.getFd(), Replier::rplChannelModeIs, replyParams);
}

void Mode::editChannelModes(const std::vector<std::string>& args, const Client& requestor,
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
            Replier::reply(requestor.getFd(), Replier::errUnknownMode, Utils::anyToVec(server.getName(),
                requestor.getNickname(), std::string(1, modes[i])));
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
            handleKeyMode(channel, action, args, argsI, requestor, server.getName(), modes[i]);
        }
        // Give/remove channel operator privilege to/from user.
        else if (modes[i] == 'o')
        {
            handleOperatorMode(channel, action, args, argsI, requestor, server.getName(), modes[i]);
        }
        // Enable+set/remove user limit on the channel.
        else if (modes[i] == 'l')
        {
            handleUserLimitMode(channel, action, args, argsI, requestor, server.getName(), modes[i]);
        }
    }
}

void Mode::handleKeyMode(Channel& channel, const std::string& action, const std::vector<std::string>& args,
    size_t& argsI, const Client& requestor, const std::string& serverName, const char mode) const
{
    if (action == "-")
    {
        channel.setKey(std::string());
        return;
    }

    if (argsI >= args.size())
    {
        Replier::reply(requestor.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(serverName,
            requestor.getNickname(), std::string(1, mode)));
        return;
    }

    const std::string& key = channel.getKey();
    if (!key.empty())
    {
        const std::string& channelName = channel.getName();
        Replier::reply(requestor.getFd(), Replier::errKeySet, Utils::anyToVec(serverName,
            requestor.getNickname(), channelName));
        return;
    }

    const std::string& newKey = args[argsI];
    channel.setKey(newKey);
}

void Mode::handleOperatorMode(Channel& channel, const std::string& action, const std::vector<std::string>& args,
    size_t& argsI, const Client& requestor, const std::string& serverName, const char mode) const
{
    if (argsI >= args.size())
    {
        Replier::reply(requestor.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(serverName,
            requestor.getNickname(), std::string(1, mode)));
        return;
    }

    const std::string& targetNickname = args[argsI];
    ++argsI;
    if (!channel.isUserOnChannel(targetNickname))
    {
        Replier::reply(requestor.getFd(), Replier::errNoSuchNick, Utils::anyToVec(serverName, targetNickname));
        return;
    }

    const bool operatorPrivilege = (action == "+");
    channel.setOperator(targetNickname, operatorPrivilege);
}

void Mode::handleUserLimitMode(Channel& channel, const std::string& action, const std::vector<std::string>& args,
    size_t& argsI, const Client& requestor, const std::string& serverName, const char mode) const
{
    if (action == "-")
    {
        channel.disableUserLimit();
        return;
    }

    if (argsI >= args.size())
    {
        Replier::reply(requestor.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(serverName,
            requestor.getNickname(), std::string(1, mode)));
        return;
    }

    const int limit = Utils::cStringToPositiveInt(args[argsI].c_str());
    ++argsI;
    channel.setUserLimit(limit);
}