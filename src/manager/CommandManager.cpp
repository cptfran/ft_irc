#include "manager/CommandManager.h"
#include "communication/Replier.h"
#include "communication/ClientTranslator.h"
#include "utils/Utils.h"
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
#include <core/Log.h>

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

/**
 * @brief Executes commands for a given client.
 * 
 * This method continuously fetches complete messages from the client's buffer
 * and executes the corresponding commands until no more messages are available.
 * 
 * @param manager Reference to the Manager instance.
 * @param client Reference to the Client instance.
 */
void CommandManager::executeCommands(Manager& manager, Client& client)
{
	while (true)
	{
		std::string msg = client.departCompleteMsgFromBuffer();
		if (msg.empty())
		{
			return;
		}
		this->executeCommand(manager, client, msg);
	}
}

/**
 * @brief Executes a single command for a given client.
 * 
 * This method parses the command and its arguments from the provided buffer,
 * checks if the command is valid, and then executes it. If the client is 
 * successfully registered and has not received welcome messages, it sends 
 * the welcome messages.
 * 
 * @param manager Reference to the Manager instance.
 * @param client Reference to the Client instance.
 * @param buffer The buffer containing the command and its arguments.
 */
void CommandManager::executeCommand(Manager& manager, Client& client, const std::string& buffer)
{
	const ConfigManager& cfg = manager.getConfigManager();

	std::pair<std::string, std::vector<std::string> > cmdWithArgs = ClientTranslator::fetchCmdAndArgs(buffer);
	if (this->validCommands.find(cmdWithArgs.first) == this->validCommands.end())
	{
		Replier::addToQueue(client.getFd(), Replier::errUnknownCommand, Utils::anyToVec(cfg.getName(),
			client.getNickname(), cmdWithArgs.first));
		return;
	}

	this->validCommands.at(cmdWithArgs.first)->execute(manager, client, cmdWithArgs.second);

	if (client.registered() && !client.getWelcomeRepliesSent())
	{
		Log::msgServer(Log::INFO, "CLIENT", client.getFd(), Log::CLIENT_REGISTER_SUCCESS);

		Replier::addToQueue(client.getFd(), Replier::rplWelcome, Utils::anyToVec(cfg.getName(), client.getNickname(),
			client.getUsername(), client.getHostname()));
		Replier::addToQueue(client.getFd(), Replier::rplYourHost, Utils::anyToVec(cfg.getName(), client.getNickname(),
			cfg.getVersion()));
		Replier::addToQueue(client.getFd(), Replier::rplCreated, Utils::anyToVec(cfg.getName(), client.getNickname(),
			cfg.getCreationDate()));
		Replier::addToQueue(client.getFd(), Replier::rplMyInfo, Utils::anyToVec(cfg.getName(), client.getNickname(),
			cfg.getVersion(), cfg.getAvailableUserModes(), cfg.getAvailableChannelModes()));

		client.setWelcomeRepliesSent(true);
	}
}