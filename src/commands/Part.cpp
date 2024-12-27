#include "commands/Part.h"

#include "Log.h"
#include "Replier.h"
#include "Server.h"
#include "Utils.h"

Part::Part()
{

}

Part::~Part()
{

}

// TODO: this command is switch off for now as it kicks user from the channel. Try to turn it on after implementing
// 'WHO'.
// Update[1]: irssi sends PART when '/window close' command is executed by user, it closes channel window and irssi
// sends PART to leave the channel
void Part::execute(Server& server, Client& client, const std::vector<std::string>& args) const
{

	if (args.empty())
	{
		Replier::reply(client.getFd(), Replier::errNeedMoreParams, Utils::anyToVec(server.getName(),
			std::string("PART")));
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
				channelToFindName));
			continue;
		}

		if (!channelToLeave->ejectUser(client.getNickname()))
		{
			Log::msgServer(INFO, "CLIENT", client.getFd(), EJECT_CLIENT_FAIL);
		}
	}
}
