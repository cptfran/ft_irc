#include "Channel.h"
#include "Log.h"
#include <algorithm>

Channel::Channel(const std::string& name)
: inviteOnly(false), topicRestricted(false), userLimitActive(false), userLimit(0)
{
	this->name = sanitizeChannelName(name);
}

Channel::Channel() : inviteOnly(false), topicRestricted(false), userLimitActive(false), userLimit(0)
{

}

Channel::~Channel()
{

}

Channel& Channel::operator=(const Channel& toCopy)
{
	if (this != &toCopy)
	{
		this->name = toCopy.name;
		this->key = toCopy.key;
		this->topic = toCopy.topic;
		this->inviteOnly = toCopy.inviteOnly;
		this->joinedClients = toCopy.joinedClients;
		this->topicRestricted = toCopy.topicRestricted;
	}
	return *this;
}

Channel::ClientData& Channel::ClientData::operator=(const ClientData& toCopy)
{
	if (this != &toCopy)
	{
		this->client = toCopy.client;
		this->isOperator = toCopy.isOperator;
	}
	return *this;
}

std::string Channel::getName() const
{
	return this->name;
}

std::string Channel::getKey() const
{
	return this->key;
}

std::vector<std::string> Channel::getNicknamesListWithOperatorInfo()
{
	std::vector<std::string> nicknamesList;

	for (std::vector<ClientData>::iterator it = this->joinedClients.begin(); it != this->joinedClients.end(); ++it)
	{
		if (it->isOperator)
		{
			nicknamesList.push_back("@" + it->client.getNickname());
		}
		else
		{
			nicknamesList.push_back(it->client.getNickname());
		}
	}

	return nicknamesList;
}

std::vector<int> Channel::getFdsList() const
{
	std::vector<int> fdsList;
	for (std::vector<Channel::ClientData>::const_iterator it = this->joinedClients.begin();
		it != this->joinedClients.end(); ++it)
	{
		fdsList.push_back(it->client.getFd());
	}
	return fdsList;
}

bool Channel::isInviteOnly() const
{
	return this->inviteOnly;
}

bool Channel::isUserInvited(const std::string& nickname) const
{
	if (std::find(this->invitedUsers.begin(), this->invitedUsers.end(), nickname) != this->invitedUsers.end())
	{
		return true;
	}
	return false;
}

bool Channel::isTopicRestricted() const
{
	return this->topicRestricted;
}

bool Channel::isUserOnChannel(const std::string& nicknameToFind) const
{
	for (std::vector<Channel::ClientData>::const_iterator it = this->joinedClients.begin();
		it != this->joinedClients.end(); ++it)
	{
		if (it->client.getNickname() == nicknameToFind)
		{
			return true;
		}
	}
	return false;
}

bool Channel::isUserOperator(const std::string& nicknameToFind) const
{
	for (std::vector<Channel::ClientData>::const_iterator it = this->joinedClients.begin();
		it != this->joinedClients.end(); ++it)
	{
		if (it->client.getNickname() == nicknameToFind && it->isOperator)
		{
			return true;
		}
	}
	return false;
}

bool Channel::isUserLimitActive() const
{
	return this->userLimitActive;
}

unsigned int Channel::getUserLimit() const
{
	return this->userLimit;
}

unsigned int Channel::getNumOfJoinedUsers() const
{
	return this->joinedClients.size();
}

std::string Channel::getTopic() const
{
	return this->topic;
}

void Channel::joinUser(Client& newClient)
{
	bool isOperator = false;
	if (this->joinedClients.empty())
	{
		isOperator = true;
	}

	const ClientData newClientData = {isOperator, newClient};
	this->joinedClients.push_back(newClientData);
}

bool Channel::ejectUser(const std::string& userToKick)
{
	for (std::vector<ClientData>::iterator it = this->joinedClients.begin(); it != this->joinedClients.end(); ++it)
	{
		if (it->client.getNickname() == userToKick)
		{
			it->client.setNumChannelsJoined(it->client.getNumChannelsJoined() - 1);
			this->joinedClients.erase(it);
			return true;
		}
	}
	return false;
}

void Channel::setChannelInviteOnly(const bool isInviteOnly)
{
	this->inviteOnly = isInviteOnly;
}

void Channel::addToInviteList(const std::string& invitedNickname)
{
	this->invitedUsers.push_back(invitedNickname);
}

void Channel::setTopicRestricted(const bool isTopicRestricted)
{
	this->topicRestricted = isTopicRestricted;
}

void Channel::setKey(const std::string& key)
{
	this->key = key;
}

void Channel::setTopic(const std::string& newTopic)
{
	this->topic = newTopic;
}

void Channel::setOperator(const std::string& targetNickname, const bool operatorPrivilege)
{
	for (std::vector<ClientData>::iterator it = this->joinedClients.begin(); it != this->joinedClients.end(); ++it)
	{
		if (it->client.getNickname() == targetNickname)
		{
			it->isOperator = operatorPrivilege;
			return;
		}
	}
}

void Channel::setUserLimit(const int limit)
{
	this->userLimit = limit;
	this->userLimitActive = true;
}

void Channel::disableUserLimit()
{
	this->userLimitActive = false;
}

std::string Channel::sanitizeChannelName(const std::string& name) const
{
	std::string sanitized = name;

	if (name[0] != '#')
	{
		sanitized = "#" + sanitized;
	}

	if (name.length() > CHANNEL_NAME_MAX_LENGTH)
	{
		sanitized = sanitized.substr(0, CHANNEL_NAME_MAX_LENGTH);
	}

	size_t pos = sanitized.find(' ');
	if (pos != std::string::npos)
	{
		sanitized = sanitized.substr(0, pos);
	}
	pos = sanitized.find(',');
	if (pos != std::string::npos)
	{
		sanitized = sanitized.substr(0, pos);
	}
	pos = sanitized.find('\a');
	if (pos != std::string::npos)
	{
		sanitized = sanitized.substr(0, pos);
	}

	return sanitized;
}
