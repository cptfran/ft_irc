#include "manager/ChannelManager.h"
#include "core/Log.h"

ChannelManager::ChannelManager() 
{

}

ChannelManager::~ChannelManager() 
{

}

Channel* ChannelManager::getNewestChannel()
{
    return &this->channels.back();
}

Channel* ChannelManager::getChannel(const std::string& channelName)
{
	for (std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); ++it)
	{
		const std::string& currChannelName = it->getName();
		if (currChannelName == channelName)
		{
			return &(*it);
		}
	}
	return NULL;
}

void ChannelManager::addChannel(const Channel& channel)
{
	channels.push_back(channel);
}

void ChannelManager::deleteClientFromChannels(const Client& client)
{
	std::vector<Channel> emptyChannels;

	for (std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end(); ++it)
	{
		if (it->isUserOnChannel(client.getNickname()))
		{
			it->deleteUser(*this, client.getNickname());

			if (it->getNumOfJoinedUsers() == 0)
			{
				emptyChannels.push_back(*it);
			}
		}
	}

	for (std::vector<Channel>::iterator it = emptyChannels.begin(); it != emptyChannels.end(); ++it)
	{
		this->deleteChannel(*it);
	}
}

void ChannelManager::deleteChannel(const Channel& channel)
{
	const std::vector<Channel>::iterator it = std::find(this->channels.begin(), this->channels.end(),
		channel);
	if (it != this->channels.end())
	{
		Log::msgServer(Log::INFO, "CHANNEL", it->getName(), Log::CHANNEL_DELETED);
		this->channels.erase(it);
	}
}

bool ChannelManager::usersHaveCommonChannel(const std::string& nickname1, const std::string& nickname2) const
{
	for (std::vector<Channel>::const_iterator it = this->channels.begin(); it != this->channels.end(); ++it)
	{
		if (it->isUserOnChannel(nickname1) && it->isUserOnChannel(nickname2))
		{
			return true;
		}
	}

	return false;
}