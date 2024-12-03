#include "../include/Channel.h"

// TODO: parse channel name properly (max 200 characters, spaces, etc.).
Channel::Channel(const std::string& name, const Client& client, const bool isOperator) :
name(name), inviteOnly(false)
{
	ClientData newClientData = {isOperator, client};
	joinedClients.push_back(newClientData);
}

Channel::Channel() : inviteOnly(false)
{

}

Channel::~Channel()
{

}

std::string Channel::getName()
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

std::string Channel::getTopic() const
{
	return this->topic;
}


void Channel::joinClient(const Client& newClient)
{
	const ClientData newClientData = {false, newClient};
	this->joinedClients.push_back(newClientData);
}
