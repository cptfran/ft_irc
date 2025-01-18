#include "data/Channel.h"
#include "core/Log.h"
#include "core/Server.h"
#include "communication/ClientTranslator.h"
#include <algorithm>

Channel::Channel(const std::string& name)
: inviteOnly(false), topicRestricted(false), userLimitActive(false), userLimit(0)
{
	this->name = ClientTranslator::sanitizeChannelName(name);
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

bool Channel::operator==(const Channel& toCompare) const
{
	return this->name == toCompare.name &&
		   this->key == toCompare.key &&
	       this->inviteOnly == toCompare.inviteOnly &&
		   this->invitedUsers == toCompare.invitedUsers &&
           this->topicRestricted == toCompare.topicRestricted &&
           this->userLimitActive == toCompare.userLimitActive &&
           this->userLimit == toCompare.userLimit &&
           this->joinedClients == toCompare.joinedClients &&
           this->topic == toCompare.topic;
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

bool Channel::ClientData::operator==(const ClientData& toCompare) const
{
	return this->isOperator == toCompare.isOperator &&
		   this->client == toCompare.client;
}


const std::string& Channel::getName() const
{
	return this->name;
}

const std::string& Channel::getKey() const
{
	return this->key;
}

const std::vector<std::string> Channel::getNicknamesListWithOperatorInfo() const
{
	std::vector<std::string> nicknamesList;

	for (std::vector<ClientData>::const_iterator it = this->joinedClients.begin(); it != this->joinedClients.end(); 
		++it)
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

const std::string& Channel::getTopic() const
{
	return this->topic;
}

const std::vector<std::string> Channel::getUserListForWhoQuery(const std::string& serverName, bool operatorOnly) const
{
	std::vector<std::string> list;

	for (std::vector<ClientData>::const_iterator it = this->joinedClients.begin(); it != this->joinedClients.end();
		++it)
	{
		if (operatorOnly && !it->isOperator)
		{
			continue;
		}

		const std::string flags = it->isOperator ? "H@" : "H";
		std::string userInfo;
		userInfo.append(this->name).append(" ")
		        .append(it->client.getUsername()).append(" ")
		        .append(it->client.getHostname()).append(" ")
		        .append(serverName).append(" ")
		        .append(it->client.getNickname()).append(" ")
		        .append(flags).append(" :0 ")
		        .append(it->client.getRealname()).append("\r\n");

		list.push_back(userInfo);
	}

	return list;
}

const std::vector<Client> Channel::getClientList() const
{
	std::vector<Client> clientList;

	for (std::vector<ClientData>::const_iterator it = this->joinedClients.begin(); it != this->joinedClients.end();
		++it)
	{
		clientList.push_back(it->client);
	}

	return clientList;
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

bool Channel::deleteUser(const std::string& userToKick)
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

void Channel::setChannelInviteOnly(bool isInviteOnly)
{
	this->inviteOnly = isInviteOnly;
}

void Channel::addToInviteList(const std::string& invitedNickname)
{
	this->invitedUsers.push_back(invitedNickname);
}

void Channel::setTopicRestricted(bool isTopicRestricted)
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

void Channel::setOperator(const std::string& targetNickname, bool operatorPrivilege)
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