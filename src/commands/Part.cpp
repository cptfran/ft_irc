#include "commands/Part.h"
#include "Log.h"
#include "Replier.h"
#include "Server.h"
#include "Utils.h"
#include "Channel.h"

Part::Part()
{

}

Part::~Part()
{

}

void Part::execute(Server& server, Client& client, const std::vector<std::string>& args) const
{
	if (args.empty())
	{
		Replier::reply(client.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(server.getName(),
			client.getNickname(), std::string("PART")));
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
			Replier::reply(client.getFd(), Replier::errNoSuchChannel, Utils::anyToVec(server.getName(),
				client.getNickname(), channelToFindName));
			continue;
		}

		if (!channelToLeave->isUserOnChannel(client.getNickname()))
		{
			Replier::reply(client.getFd(), Replier::errNotOnChannel, Utils::anyToVec(server.getName(),
				client.getNickname(), channelToFindName));
			continue;
		}

		if (!channelToLeave->ejectUser(server, client.getNickname()))
		{
			Log::msgServer(INFO, "CLIENT", client.getFd(), EJECT_CLIENT_FAIL);
		}
	}
}
