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

void Part::execute(Manager& serverManager, Client& requester, const std::vector<std::string>& args) const
{
	ConfigManager& configManager = serverManager.getConfigManager();

	if (args.empty())
	{
		Replier::addToQueue(requester.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(configManager.getName(),
			requester.getNickname(), std::string("PART")));
		return;
	}

	const std::string& channelsToLeaveStacked = args[0];
	std::vector<std::string> channelsToLeaveSplit = Utils::splitStringByComma(channelsToLeaveStacked);

	for (std::vector<std::string>::iterator it = channelsToLeaveSplit.begin(); it != channelsToLeaveSplit.end(); ++it)
	{
		const std::string& channelToFindName = *it;
		Channel* channelToLeave = serverManager.getChannelManager().getChannel(channelToFindName);

		if (channelToLeave == NULL)
		{
			Replier::addToQueue(requester.getFd(), Replier::errNoSuchChannel, Utils::anyToVec(configManager.getName(),
				requester.getNickname(), channelToFindName));
			continue;
		}

		if (!channelToLeave->isUserOnChannel(requester.getNickname()))
		{
			Replier::addToQueue(requester.getFd(), Replier::errNotOnChannel, Utils::anyToVec(configManager.getName(),
				requester.getNickname(), channelToFindName));
			continue;
		}

		if (!channelToLeave->deleteUser(requester.getNickname()))
		{
			Log::msgServer(INFO, "CLIENT", requester.getFd(), EJECT_CLIENT_FAIL);
		}
	}
}
