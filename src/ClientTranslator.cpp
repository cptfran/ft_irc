#include <map>
#include "../include/ClientTranslator.h"
#include <sstream>
#include <algorithm>
#include <sys/socket.h>
#include "Log.h"
#include "Utils.h"

ClientTranslator::ClientTranslator()
{

}

ClientTranslator::~ClientTranslator()
{

}

std::map<std::string, std::vector<std::string> > ClientTranslator::fetchCommands(const std::string& buffer,
	const std::map<std::string, Command*>& validServerCommands)
{
	// Split buffer to tokens.
	std::istringstream iss(buffer);
	std::string token;
	std::vector<std::string> tokens;

	while (iss >> token)
	{
		if (!token.empty() && token[0] == ':')
		{
			std::string restOfBuffer;
			std::getline(iss, restOfBuffer);
			token += restOfBuffer;
			if (token.length() > 2)
			{
				tokens.push_back(token);
			}
			break;
		}
		tokens.push_back(token);
	}
	if (tokens.empty())
	{
		throw std::runtime_error(ERROR EMPTY_CLIENT_PROMPT);
	}

	std::map<std::string, std::vector<std::string> > fetchedCommands;

	// Separate commands and assign them with associating arguments.
	while (!tokens.empty())
	{
		// : semicolon means, that everything after it is one message/argument, even if there are spaces,
		// for example, realname in USER command starts with ":" so everything after it should be saved to the realname.
		// Need to implement it.
		const std::string command = *tokens.begin();
		std::vector<std::string> arguments;
		std::vector<std::string>::iterator it = tokens.begin() + 1;
		for (; it != tokens.end(); ++it)
		{
			if (validServerCommands.find(*it) != validServerCommands.end())
			{
				break;
			}
			arguments.push_back(*it);
		}
		tokens.erase(tokens.begin(), it);
		fetchedCommands.insert(make_pair(command, arguments));
	}
	return fetchedCommands;
}

std::vector<std::string> ClientTranslator::extractPrivmsgTargets(const std::string& targets)
{
    // Split targets by commas.
    std::istringstream iss(targets);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(iss, token, ','))
    {
        tokens.push_back(token);
    }

    return tokens;
}
