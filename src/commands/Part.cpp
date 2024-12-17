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
	const std::string& serverName = server.getName();
	const int clientFd = client.getFd();

	if (args.empty())
	{
		const std::string command = "PARt";
		Replier::reply(clientFd, Replier::errNeedMoreParams, Utils::anyToVec(serverName, command));
		return;
	}

	const std::string& channelsToLeaveStack = args[0];
	std::vector<std::string> channelsToLeaveSplit = Utils::splitStringByComma(channelsToLeaveStack);

	for (std::vector<std::string>::iterator it = channelsToLeaveSplit.begin(); it != channelsToLeaveSplit.end(); ++it)
	{
		const std::string& channelToFindName = *it;
		Channel* channelToLeave;
		try
		{
			channelToLeave = &server.getChannel(channelToFindName);
		}
		catch (const std::exception&)
		{
			Replier::reply(clientFd, Replier::errNoSuchChannel, Utils::anyToVec(serverName, channelToFindName));
			continue;
		}

		if (!channelToLeave->isUserOnChannel(channelToFindName))
		{
			Replier::reply(clientFd, Replier::errNotOnChannel, Utils::anyToVec(serverName, channelToFindName));
			continue;
		}

		const std::string& clientNickname = client.getNickname();
		if (!channelToLeave->ejectUser(clientNickname))
		{
			Log::msgServer(INFO, "CLIENT", clientFd, EJECT_CLIENT_FAIL);
		}
	}
}
