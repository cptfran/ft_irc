#include "Channel.h"
#include "Log.h"

// TODO: parse channel name properly (max 200 characters, spaces, etc.).
Channel::Channel(const std::string& name) : name(name), inviteOnly(false), topicRestricted(false)
{

}

Channel::Channel() : inviteOnly(false), topicRestricted(false)
{

}

Channel::~Channel()
{

}

std::string Channel::getName() const
{
	return this->name;
}

std::vector<std::string> Channel::getNicknamesListWithOperatorInfo()
{
	std::vector<std::string> nicknamesList;

	for (std::vector<ClientData>::iterator it = this->joinedClients.begin(); it != this->joinedClients.end(); ++it)
	{
		if (it->isOperator)
		{
			nicknamesList.push_back("@" + it->client->getNickname());
		}
		else
		{
			nicknamesList.push_back(it->client->getNickname());
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
		fdsList.push_back(it->client->getFd());
	}
	return fdsList;
}

bool Channel::isInviteOnly() const
{
	return this->inviteOnly;
}

bool Channel::isTopicRestricted() const
{
	return this->topicRestricted;
}

std::string Channel::getTopic() const
{
	return this->topic;
}

Channel::ClientData* Channel::findClientData(const Client& clientToFind)
{
	for (std::vector<ClientData>::iterator it = this->joinedClients.begin(); it != this->joinedClients.end();
		++it)
	{
		if (*it->client == clientToFind)
		{
			return &(*it);
		}
	}
	return NULL;
}

void Channel::joinClient(Client& newClient)
{
	bool isOperator = false;
	if (this->joinedClients.empty())
	{
		isOperator = true;
	}

	const ClientData newClientData = {isOperator, &newClient};
	this->joinedClients.push_back(newClientData);
}

bool Channel::ejectClient(const std::string& userToKick)
{
	for (std::vector<ClientData>::iterator it = this->joinedClients.begin(); it != this->joinedClients.end(); ++it)
	{
		if (it->client->getNickname() == userToKick)
		{
			it->client->setNumChannelsJoined(it->client->getNumChannelsJoined() - 1);
			this->joinedClients.erase(it);
			return true;
		}
	}
	return false;
}

void Channel::setTopic(const std::string& newTopic)
{
	this->topic = newTopic;
}
