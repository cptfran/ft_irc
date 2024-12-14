#pragma once

#include <string>
#include <vector>
#include "Client.h"

// PROBABLY rules for joining:
// 1.  the user must be invited if the channel is invite-only;
// 2.  the user's nick/username/hostname must not match any active bans;
// 3.  the correct key (password) must be given if it is set.

// A channel is a named group of one or more clients which will all
//    receive messages addressed to that channel.  The channel is created
//    implicitly when the first client joins it, and the channel ceases to
//    exist when the last client leaves it.  While channel exists, any
//    client can reference the channel using the name of the channel.
//
//    Channels names are strings (beginning with a '&' or '#' character) of
//    length up to 200 characters.  Apart from the the requirement that the
//    first character being either '&' or '#'; the only restriction on a
//    channel name is that it may not contain any spaces (' '), a control G
//    (^G or ASCII 7), or a comma (',' which is used as a list item
//    separator by the protocol).
class Channel
{
public:
	struct ClientData
	{
		bool isOperator;
		Client& client;
		ClientData& operator=(const ClientData& toCopy);
	};

	explicit Channel(const std::string& name);
	Channel& operator=(const Channel& toCopy);
	~Channel();

	std::string getName() const;
	std::string getKey() const;
	std::vector<std::string> getNicknamesListWithOperatorInfo();
	std::vector<int> getFdsList() const;
	bool isInviteOnly() const;
	bool isUserInvited(const std::string& nickname) const;
	bool isTopicRestricted() const;
	bool isClientOnChannel(const std::string& nicknameToFind) const;
	bool isClientOperator(const Client& clientToFind) const;
	bool isUserLimitActive() const;
	int getUserLimit() const;
	std::string getTopic() const;
	ClientData& findClientData(const Client& clientToFind);

	void joinClient(Client& newClient);
	bool ejectClient(const std::string& userToKick);
	void setChannelInviteOnly(bool isInviteOnly);
	void addToInviteList(const std::string& invitedNickname);
	void setTopicRestricted(bool isTopicRestricted);
	void setKey(const std::string& key);
	void setTopic(const std::string& newTopic);
	void setOperator(const std::string& targetNickname, bool operatorPrivilege);
	void setUserLimit(int limit);
	void disableUserLimit();

private:
	std::string name;
	std::string key;
	bool inviteOnly;
	std::vector<std::string> invitedUsers;
	bool topicRestricted;
	bool userLimitActive;
	int userLimit;
	std::vector<ClientData> joinedClients;
	std::string topic;

	Channel();
};
