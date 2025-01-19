#pragma once

#include <vector>
#include <string>
#include "data/Channel.h"
#include "data/Client.h"

class ChannelManager 
{
public:
    ChannelManager();
    ~ChannelManager();
   
    Channel* getNewestChannel();
    Channel* getChannel(const std::string& channelName);
    bool usersHaveCommonChannel(const std::string& nickname1, const std::string& nickname2) const;

    void addChannel(const Channel& channel);
    void deleteChannel(const Channel& channel);
    void deleteClientFromChannels(const Client& client);

private:
    ChannelManager(const ChannelManager& toCopy);
    ChannelManager& operator=(const ChannelManager& toAssign);

    std::vector<Channel> channels;
};