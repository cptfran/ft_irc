#include "../../include/commands/Join.h"
#include "../../include/Server.h"
#include "../../include/Utils.h"
#include "../../include/Replier.h"
#include <algorithm>

Join::Join()
{

}

Join::~Join()
{

}

void Join::execute(Server& server, Client& client, const std::vector<std::string>& args) const
{
	// TODO: handle this command properly. This is just a test code.

	const std::string channelName = args[0];
	std::vector<Channel> availableChannels = server.getChannels();
	const std::vector<Channel>::iterator it = std::find(availableChannels.begin(), availableChannels.end(),
		channelName);
	if (it != availableChannels.end() && it->isInviteOnly())
	{
		Replier::reply(client, Replier::errInviteOnlyChan, Utils::anyToVec(server.getName(), channelName));
	}

	if (!client.registered(server.getPassword()))
	{
		Replier::reply(client, Replier::errNotRegistered, Utils::anyToVec(std::string("")));
		return;
	}
	Replier::reply(client, Replier::rplNoTopic, Utils::anyToVec(args[0]));
}
