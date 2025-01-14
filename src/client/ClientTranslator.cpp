#include <map>
#include <cctype>
#include "client/ClientTranslator.h"
#include <sstream>
#include <algorithm>
#include <sys/socket.h>
#include "server/Log.h"
#include "utils/Utils.h"

ClientTranslator::ClientTranslator()
{

}

ClientTranslator::~ClientTranslator()
{

}

std::vector<std::pair<std::string, std::vector<std::string> > > ClientTranslator::fetchCommands(
	const std::string& buffer, const std::map<std::string, Command*>& validServerCommands)
{
	// Split buffer to tokens.
	std::istringstream iss(buffer);
	std::string token;
	std::vector<std::string> tokens;

	std::cout << "buffer in fetcher: " << buffer << std::endl;
	while (iss >> token)
	{
		if (!token.empty() && token[0] == ':')
		{
			std::string colonMessage;
			std::getline(iss, colonMessage, '\r');
			token += colonMessage;
			if (token.length() > 2)
			{
				tokens.push_back(token);
			}
			continue;
		}
		std::cout << "token: " << token << std::endl;
		tokens.push_back(token);
	}
	if (tokens.empty())
	{
		throw std::runtime_error(ERROR EMPTY_CLIENT_PROMPT);
	}

	std::vector<std::pair<std::string, std::vector<std::string> > > fetchedCommands;

	// Separate commands and assign them with associating arguments.
	while (!tokens.empty())
	{
		const std::string command = Utils::stringToUpper(*tokens.begin());
		std::vector<std::string> arguments;
		std::vector<std::string>::iterator it = tokens.begin() + 1;
		for (; it != tokens.end(); ++it)
		{
			if (validServerCommands.find(Utils::stringToUpper(*it)) != validServerCommands.end())
			{
				break;
			}
			arguments.push_back(*it);
		}
		tokens.erase(tokens.begin(), it);
		fetchedCommands.push_back(make_pair(command, arguments));
	}
	return fetchedCommands;
}

bool ClientTranslator::nicknameValid(const std::string& nickname)
{
	const std::string specialChars = "[]{}\\|^\'_";

	bool lengthValid = (nickname.length() <= 9);
	bool firstCharValid = (isalpha(nickname[0]) || specialChars.find(nickname[0]) != std::string::npos);
	bool restOfCharsValid = true;
	for (std::string::const_iterator it = nickname.begin() + 1; it != nickname.end(); ++it)
	{
		if (specialChars.find(*it) == std::string::npos && *it != '-' && !isdigit(*it) && !isalpha(*it))
		{
			restOfCharsValid = false;
			break;
		}
	}

	return lengthValid && firstCharValid && restOfCharsValid;
}

std::vector<std::string> ClientTranslator::splitToTokens(const std::string& str, const char splitter)
{
    // Split targets by commas.
    std::istringstream iss(str);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(iss, token, splitter))
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

std::string ClientTranslator::sanitizeChannelName(const std::string& name)
{
	std::string sanitized = name;

	if (name[0] != '#')
	{
		sanitized = "#" + sanitized;
	}

	if (name.length() > CHANNEL_NAME_MAX_LENGTH)
	{
		sanitized = sanitized.substr(0, CHANNEL_NAME_MAX_LENGTH);
	}

	size_t pos = sanitized.find(' ');
	if (pos != std::string::npos)
	{
		sanitized = sanitized.substr(0, pos);
	}
	pos = sanitized.find(',');
	if (pos != std::string::npos)
	{
		sanitized = sanitized.substr(0, pos);
	}
	pos = sanitized.find('\a');
	if (pos != std::string::npos)
	{
		sanitized = sanitized.substr(0, pos);
	}

	return sanitized;
}

void ClientTranslator::parseUserTarget(const std::string& extrTarget, std::string& nickname, std::string& username,
	std::string& hostname, std::string& serverName)
{
	std::vector<char> splitters = Utils::anyToVec('!', '%', '@');

	assignUserInfo(extrTarget, splitters.begin(), splitters.end(), nickname, username, hostname, serverName);
	if (nickname.empty() && username.empty() && hostname.empty() && serverName.empty())
	{
		nickname = extrTarget;
	}
}

void ClientTranslator::assignUserInfo(const std::string extrTarget, std::vector<char>::iterator splitter,
	std::vector<char>::iterator splittersEnd, std::string& nickname, std::string& username, std::string& hostname,
	std::string& serverName)
{
	if (splitter == splittersEnd)
	{
		return;
	}

	const size_t splitPos = extrTarget.find(*splitter);
	if (splitPos != std::string::npos)
	{
		if (*splitter == '!')
		{
			nickname = extrTarget.substr(0, splitPos);
			username = extrTarget.substr(splitPos + 1);
			assignUserInfo(username, ++splitter, splittersEnd, nickname, username, hostname, serverName);
		}
		else if (*splitter == '%')
		{

			username = extrTarget.substr(0, splitPos);
			hostname = extrTarget.substr(splitPos + 1);
			assignUserInfo(hostname, ++splitter, splittersEnd, nickname, username, hostname, serverName);
		}
		else if (*splitter == '@')
		{
			if (!hostname.empty())
			{
				hostname = extrTarget.substr(0, splitPos);
				serverName = extrTarget.substr(splitPos + 1);
			}
			else
			{
				username = extrTarget.substr(0, splitPos);
				hostname = extrTarget.substr(splitPos + 1);
				serverName = extrTarget.substr(splitPos + 1);
			}
			assignUserInfo(hostname, ++splitter, splittersEnd, nickname, username, hostname, serverName);
		}
	}
	else
	{
		assignUserInfo(extrTarget, ++splitter, splittersEnd, nickname, username, hostname, serverName);
	}
}