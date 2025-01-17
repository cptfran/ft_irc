#include "manager/ChannelManager.h"

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

void ChannelManager::deleteChannelIfEmpty(const Channel& channel)
{
	if (channel.getNumOfJoinedUsers() == 0)
	{
		const std::vector<Channel>::iterator it = std::find(this->channels.begin(), this->channels.end(),
			channel);
		if (it != this->channels.end())
		{
			this->channels.erase(it);
		}
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