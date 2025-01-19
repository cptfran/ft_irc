#include "commands/Part.h"
#include "core/Log.h"
#include "communication/Replier.h"
#include "core/Server.h"
#include "utils/Utils.h"
#include "data/Channel.h"

Part::Part() : Command()
{

}

Part::~Part()
{

}

/**
 * @brief Executes the PART command which allows a client to leave one or more channels.
 * 
 * @param serverManager Reference to the server manager handling the command.
 * @param requester Reference to the client requesting the command.
 * @param args Vector of arguments passed to the command.
 */
void Part::execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const
{
    ConfigManager& configManager = serverManager.getConfigManager();

    // Check if arguments are provided.
    if (args.empty())
    {
        Replier::addToQueue(requester.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(configManager.getName(),
            requester.getNickname(), std::string("PART")));
        return;
    }

    const std::string& channelsToLeaveStacked = args[0];
    std::vector<std::string> channelsToLeaveSplit = Utils::splitStringByComma(channelsToLeaveStacked);

    // Iterate through the list of channels to leave.
    for (std::vector<std::string>::iterator it = channelsToLeaveSplit.begin(); it != channelsToLeaveSplit.end(); ++it)
    {
        const std::string& channelToFindName = *it;
        Channel* channelToLeave = serverManager.getChannelManager().getChannel(channelToFindName);

        // Check if the channel exists
        if (channelToLeave == NULL)
        {
            Replier::addToQueue(requester.getFd(), Replier::errNoSuchChannel, Utils::anyToVec(configManager.getName(),
                requester.getNickname(), channelToFindName));
            continue;
        }

        // Check if the user is on the channel.
        if (!channelToLeave->isUserOnChannel(requester.getNickname()))
        {
            Replier::addToQueue(requester.getFd(), Replier::errNotOnChannel, Utils::anyToVec(configManager.getName(),
                requester.getNickname(), channelToFindName));
            continue;
        }

        // Attempt to remove the user from the channel.
        if (!channelToLeave->deleteUser(requester.getNickname()))
        {
            Log::msgServer(Log::INFO, "CLIENT", requester.getFd(), Log::EJECT_CLIENT_FAIL);
        }
    }
}
