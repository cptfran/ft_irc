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

bool ClientTranslator::matchWildcard(const char* pattern, const char* str)
{
	// Base case: if we reach the end of both the pattern and the string, it's a match
	if (*pattern == '\0' && *str == '\0')
	{
		return true;
	}

	// If the current character in the pattern is '*', it can match zero or more characters in the string
	if (*pattern == '*')
	{
		// Try to match the rest of the pattern with the current string or the rest of the string
		return matchWildcard(pattern + 1, str) || (*str != '\0' && matchWildcard(pattern, str + 1));
	}

	// If the current character in the pattern is '?', it can match any single character in the string
	if (*pattern == '?' || *pattern == *str)
	{
		return matchWildcard(pattern + 1, str + 1);
	}

	// If the current characters in the pattern and the string do not match, it's not a match
	return false;
}


std::string ClientTranslator::sanitizeColonMessage(const std::string& message)
{
	std::string sanitized;

	if (!message.empty() && message[0] == ':')
	{
		sanitized = message.substr(1);
	}

	for (std::string::reverse_iterator it = sanitized.rbegin(); it != sanitized.rend(); ++it)
	{
		if (*it != '\r' && *it != '\n')
		{
			const std::string::size_type cutPos = sanitized.rend() - it;
			return sanitized.substr(0, cutPos);
		}
	}

	return sanitized;
}