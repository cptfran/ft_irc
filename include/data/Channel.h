#pragma once

#include <string>
#include <vector>

class ChannelManager;
class Client;

class Channel
{
public:
	struct ClientData
	{
		bool isOperator;
		Client& client;
		ClientData& operator=(const ClientData& toCopy);
		bool operator==(const ClientData& toCompare) const;
	};

	explicit Channel(const std::string& name);
	Channel& operator=(const Channel& toCopy);
	bool operator==(const Channel& toCompare) const;
	~Channel();

	const std::string& getName() const;
	const std::string& getKey() const;
	const std::vector<std::string> getNicknamesListWithOperatorInfo() const;
	bool isInviteOnly() const;
	bool isUserInvited(const std::string& nickname) const;
	bool isTopicRestricted() const;
	bool isUserOnChannel(const std::string& nicknameToFind) const;
	bool isUserOperator(const std::string& nicknameToFind) const;
	bool isUserLimitActive() const;
	unsigned int getUserLimit() const;
	unsigned int getNumOfJoinedUsers() const;
	const std::string& getTopic() const;
	const std::vector<std::string> getUserListForWhoQuery(const std::string& serverName, bool operatorOnly) const;
	const std::vector<Client> getClientList() const;

	void joinUser(Client& newClient);
	bool deleteUser(ChannelManager& channelManager, const std::string& userToKick);
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
