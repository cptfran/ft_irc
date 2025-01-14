#include "commands/Part.h"
#include "server/Log.h"
#include "replier/Replier.h"
#include "server/Server.h"
#include "utils/Utils.h"
#include "channel/Channel.h"

Part::Part()
{

}

Part::~Part()
{

}

void Part::execute(Server& server, Client& requester, const std::vector<std::string>& args) const
{
	if (args.empty())
	{
		Replier::reply(requester.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(server.getName(),
			requester.getNickname(), std::string("PART")));
		return;
	}

	const std::string& channelsToLeaveStacked = args[0];
	std::vector<std::string> channelsToLeaveSplit = Utils::splitStringByComma(channelsToLeaveStacked);

	for (std::vector<std::string>::iterator it = channelsToLeaveSplit.begin(); it != channelsToLeaveSplit.end(); ++it)
	{
		const std::string& channelToFindName = *it;
		Channel* channelToLeave = server.getChannel(channelToFindName);

		if (channelToLeave == NULL)
		{
			Replier::reply(requester.getFd(), Replier::errNoSuchChannel, Utils::anyToVec(server.getName(),
				requester.getNickname(), channelToFindName));
			continue;
		}

		if (!channelToLeave->isUserOnChannel(requester.getNickname()))
		{
			Replier::reply(requester.getFd(), Replier::errNotOnChannel, Utils::anyToVec(server.getName(),
				requester.getNickname(), channelToFindName));
			continue;
		}

		if (!channelToLeave->ejectUser(server, requester.getNickname()))
		{
			Log::msgServer(INFO, "CLIENT", requester.getFd(), EJECT_CLIENT_FAIL);
		}
	}
}
