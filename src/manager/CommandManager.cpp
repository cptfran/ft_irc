#include "manager/CommandManager.h
#include "commands/Cap.h"
#include "commands/Join.h"
#include "commands/Nick.h"
#include "commands/Pass.h"
#include "commands/Ping.h"
#include "commands/User.h"
#include "commands/Kick.h"
#include "commands/Topic.h"
#include "commands/Invite.h"
#include "commands/Mode.h"
#include "commands/Part.h"
#include "commands/Who.h"
#include "commands/Privmsg.h"

CommandManager::CommandManager()
{
	this->validCommands["CAP"] = new Cap();
	this->validCommands["INVITE"] = new Invite();
	this->validCommands["JOIN"] = new Join();
	this->validCommands["KICK"] = new Kick();
	this->validCommands["MODE"] = new Mode();
	this->validCommands["NICK"] = new Nick();
	this->validCommands["PART"] = new Part();
	this->validCommands["PASS"] = new Pass();
	this->validCommands["PING"] = new Ping();
	this->validCommands["PRIVMSG"] = new Privmsg();
	this->validCommands["TOPIC"] = new Topic();
	this->validCommands["USER"] = new User();
	this->validCommands["WHO"] = new Who();
}

CommandManager::~CommandManager()
{
	for (std::map<std::string, Command*>::iterator it = this->validCommands.begin(); it != this->validCommands.end();
		++it)
	{
		delete it->second;
	}
}

void CommandManager::executeCommand(Client& client, const std::string& buffer)
{
	std::pair<std::string, std::vector<std::string> > cmdWithArgs = ClientTranslator::fetchCmdAndArgs(buffer);
	if (this->validCommands.find(cmdWithArgs.first) == this->validCommands.end())
	{
		Replier::addToQueue(client.getFd(), Replier::errUnknownCommand, Utils::anyToVec(this->name,
			client.getNickname(), cmdWithArgs.first));
		return;
	}

	this->validCommands.at(cmdWithArgs.first)->execute(*this, client, cmdWithArgs.second);

	if (client.registered(this->password) && !client.getWelcomeRepliesSent())
	{
		Log::msgServer(INFO, "CLIENT", client.getFd(), CLIENT_REGISTER_SUCCESS);

		Replier::addToQueue(client.getFd(), Replier::rplWelcome, Utils::anyToVec(this->name, client.getNickname(),
			client.getUsername(), client.getHostname()));
		Replier::addToQueue(client.getFd(), Replier::rplYourHost, Utils::anyToVec(this->name, client.getNickname(),
			this->version));
		Replier::addToQueue(client.getFd(), Replier::rplCreated, Utils::anyToVec(this->name, client.getNickname(),
			this->creationDate));
		Replier::addToQueue(client.getFd(), Replier::rplMyInfo, Utils::anyToVec(this->name, client.getNickname(),
			this->version, this->availableUserModes, this->availableChannelModes));

		client.setWelcomeRepliesSent(true);
	}
}