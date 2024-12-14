#include "Channel.h"
#include "Log.h"
#include <algorithm>

// TODO: parse channel name properly (max 200 characters, spaces, etc.).
Channel::Channel(const std::string& name)
: name(name), inviteOnly(false), topicRestricted(false), userLimitActive(false), userLimit(0)
{

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

bool Channel::isClientOnChannel(const std::string& nicknameToFind) const
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

bool Channel::isClientOperator(const Client& clientToFind) const
{
	for (std::vector<Channel::ClientData>::const_iterator it = this->joinedClients.begin();
		it != this->joinedClients.end(); ++it)
	{
		if (it->client == clientToFind && it->isOperator)
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

int Channel::getUserLimit() const
{
	return this->userLimit;
}

std::string Channel::getTopic() const
{
	return this->topic;
}

Channel::ClientData& Channel::findClientData(const Client& clientToFind)
{
	for (std::vector<ClientData>::iterator it = this->joinedClients.begin(); it != this->joinedClients.end(); ++it)
	{
		if (it->client == clientToFind)
		{
			return *it;
		}
	}
	throw std::runtime_error("Client not found.");
}

void Channel::joinClient(Client& newClient)
{
	bool isOperator = false;
	if (this->joinedClients.empty())
	{
		isOperator = true;
	}

	const ClientData newClientData = {isOperator, newClient};
	this->joinedClients.push_back(newClientData);
}

bool Channel::ejectClient(const std::string& userToKick)
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
