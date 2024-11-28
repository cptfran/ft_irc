#include "../include/Channel.h"

Channel::Channel(const std::string& name) : name(name), inviteOnly(false)
{

}

Channel::Channel()
{

}

Channel::~Channel()
{

}

bool Channel::isInviteOnly() const
{
	return this->inviteOnly;
}
