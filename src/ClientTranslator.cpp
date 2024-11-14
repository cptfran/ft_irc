#include <map>
#include "../include/ClientTranslator.h"
#include <sstream>
#include <algorithm>
#include <sys/socket.h>
#include "Client.h"
#include "Log.h"

ClientTranslator::ClientTranslator(const std::string& buffer) : buffer(buffer)
{

}

ClientTranslator::~ClientTranslator()
{

}

// void ClientTranslator::fetchCommands(const Server& server, const Client& client)
// {
// 	// Split buffer to tokens.
// 	std::istringstream iss(this->buffer);
// 	std::string token;
// 	std::vector<std::string> tokens;
//
// 	while (iss >> token)
// 	{
// 		tokens.push_back(token);
// 	}
// 	if (tokens.empty())
// 	{
// 		throw std::runtime_error(ERROR EMPTY_CLIENT_PROMPT);
// 	}
//
// 	// Init list of available commands.
// 	const std::string commandList[] = {"CAP", "JOIN", "PASS", "NICK", "USER"};
// 	const std::string* commandListFirst = commandList;
// 	const std::string* commandListLast = commandList + sizeof(commandList) / sizeof(commandList[0]);
//
// 	// Separate commands and assign them with associating arguments.
// 	while (!tokens.empty())
// 	{
// 		// TODO: semicolon means, that everything after it is one message/argument, even if there are spaces,
// 		// for example, realname in USER command starts with ":" so everything after it should be saved to the realname.
// 		// Need to implement it.
// 		if (find(commandListFirst, commandListLast, tokens[0]) != commandListLast)
// 		{
// 			const std::string command = *tokens.begin();
// 			std::vector<std::string> arguments;
// 			std::vector<std::string>::iterator it = tokens.begin() + 1;
// 			for (; it != tokens.end(); ++it)
// 			{
// 				if (find(commandListFirst, commandListLast, *it) != commandListLast)
// 				{
// 					break;
// 				}
// 				arguments.push_back(*it);
// 			}
// 			tokens.erase(tokens.begin(), it);
// 			this->commands.insert(make_pair(command, arguments));
// 		}
// 		else
// 		{
// 			std::vector<std::string> args;
// 			args.push_back(tokens[0]);
// 			server.reply(client, 421, args);
// 			tokens.erase(tokens.begin());
// 		}
// 	}
// }

void ClientTranslator::fetchCommands()
{
	// Split buffer to tokens.
	std::istringstream iss(this->buffer);
	std::string token;
	std::vector<std::string> tokens;

	while (iss >> token)
	{
		tokens.push_back(token);
	}
	if (tokens.empty())
	{
		throw std::runtime_error(ERROR EMPTY_CLIENT_PROMPT);
	}

	// Init list of available commands.
	const std::string commandList[] = {"CAP", "JOIN", "PASS", "NICK", "USER", "PING"};
	const std::string* commandListFirst = commandList;
	const std::string* commandListLast = commandList + sizeof(commandList) / sizeof(commandList[0]);

	// Separate commands and assign them with associating arguments.
	while (!tokens.empty())
	{
		// TODO: semicolon means, that everything after it is one message/argument, even if there are spaces,
		// for example, realname in USER command starts with ":" so everything after it should be saved to the realname.
		// Need to implement it.
		const std::string command = *tokens.begin();
		std::vector<std::string> arguments;
		std::vector<std::string>::iterator it = tokens.begin() + 1;
		for (; it != tokens.end(); ++it)
		{
			if (find(commandListFirst, commandListLast, *it) != commandListLast)
			{
				break;
			}
			arguments.push_back(*it);
		}
		tokens.erase(tokens.begin(), it);
		this->commands.insert(make_pair(command, arguments));
	}
}

std::string ClientTranslator::getBuffer() const
{
	return this->buffer;
}

std::map<std::string, std::vector<std::string> > ClientTranslator::getCommands() const
{
	return this->commands;
}
