#include <map>
#include <cctype>
#include "communication/ClientTranslator.h"
#include <sstream>
#include <algorithm>
#include <sys/socket.h>
#include "core/Log.h"
#include "utils/Utils.h"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include "manager/ConfigManager.h"

ClientTranslator::ClientTranslator()
{

}

ClientTranslator::~ClientTranslator()
{

}

/**
 * @brief Parses the client buffer received from the given file descriptor.
 *
 * This method reads data from the specified file descriptor into a buffer and returns it as a string.
 * If no data is read, it checks for specific Log::ERROR conditions and throws an exception if necessary.
 *
 * @param fd The file descriptor from which to receive the buffer.
 * @return The received buffer as a string.
 */
std::string ClientTranslator::parseClientBufferFromRecv(const int fd)
{
	char recvBuffer[ConfigManager::INPUT_BUFFER_SIZE] = {};
	const ssize_t bytesRead = recv(fd, recvBuffer, sizeof(recvBuffer) - 1, 0);
	DEBUG_LOG(std::string("CLIENT[" + Utils::intToString(fd) + "]: \"") + recvBuffer + "\"");
	if (bytesRead <= 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			return std::string();
		}
		throw std::runtime_error(Log::ERROR + std::string("recv"));
	}
	recvBuffer[bytesRead] = '\0';

	return recvBuffer;
}

/**
 * @brief Fetches the command and arguments from the given buffer.
 *
 * This method splits the buffer into tokens, extracts the command (first token), and processes the remaining tokens
 * as arguments.
 *
 * @param buffer The buffer containing the command and arguments.
 * @return A pair containing the command and a vector of arguments.
 */
std::pair<std::string, std::vector<std::string> > ClientTranslator::fetchCmdAndArgs(const std::string& buffer)
{
	// Split buffer into tokens.
	std::istringstream iss(buffer);
	std::string token;
	std::vector<std::string> args;

	// Extract the command (first token).
	std::string cmd;
	iss >> cmd;
	cmd = Utils::stringToUpper(cmd);

	// Process remaining tokens (arguments).
	while (iss >> token)
	{
		if (!token.empty() && token[0] == ':')
		{
			std::string colonMessage;
			std::getline(iss, colonMessage, '\r');
			token += colonMessage;
			if (token.length() > 1)
			{
				args.push_back(token);
			}
			break;
		}
		args.push_back(token);
	}

	return std::make_pair(cmd, args);
}

/**
 * @brief Validates the given nickname according to specific rules.
 *
 * This method checks if the nickname meets the length, first character, and character set requirements.
 *
 * @param nickname The nickname to validate.
 * @return True if the nickname is valid, false otherwise.
 */
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

/**
 * @brief Splits the given string into tokens using the specified splitter character.
 *
 * This method splits the input string into tokens based on the provided delimiter and returns them as a vector.
 *
 * @param str The string to split.
 * @param splitter The character to use as the delimiter.
 * @return A vector of tokens.
 */
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

/**
 * @brief Matches the given string against the specified wildcard pattern.
 *
 * This method checks if the input string matches the wildcard pattern, where '*' matches zero or more characters and '?' matches any single character.
 *
 * @param pattern The wildcard pattern to match against.
 * @param str The string to match.
 * @return True if the string matches the pattern, false otherwise.
 */
bool ClientTranslator::matchWildcard(const char* pattern, const char* str)
{
	// Base case: if we reach the end of both the pattern and the string, it's a match.
	if (*pattern == '\0' && *str == '\0')
	{
		return true;
	}

	// If the current character in the pattern is '*', it can match zero or more characters in the string.
	if (*pattern == '*')
	{
		// Try to match the rest of the pattern with the current string or the rest of the string
		return matchWildcard(pattern + 1, str) || (*str != '\0' && matchWildcard(pattern, str + 1));
	}

	// If the current character in the pattern is '?', it can match any single character in the string.
	if (*pattern == '?' || *pattern == *str)
	{
		return matchWildcard(pattern + 1, str + 1);
	}

	// If the current characters in the pattern and the string do not match, it's not a match.
	return false;
}

/**
 * @brief Sanitizes the given message by removing the leading colon and trailing CRLF characters.
 *
 * This method removes the leading colon from the message and trims any trailing carriage return and line feed characters.
 *
 * @param message The message to sanitize.
 * @return The sanitized message.
 */
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

/**
 * @brief Sanitizes the given channel name by ensuring it starts with '#' and removing invalid characters.
 *
 * This method ensures the channel name starts with a '#' character and removes any invalid characters or truncates it to the maximum length.
 *
 * @param name The channel name to sanitize.
 * @return The sanitized channel name.
 */
std::string ClientTranslator::sanitizeChannelName(const std::string& name)
{
	std::string sanitized = name;

	if (name[0] != '#')
	{
		sanitized = "#" + sanitized;
	}

	if (name.length() > ConfigManager::CHANNEL_NAME_MAX_LENGTH)
	{
		sanitized = sanitized.substr(0, ConfigManager::CHANNEL_NAME_MAX_LENGTH);
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

/**
 * @brief Parses the user target string into nickname, username, hostname, and serverName.
 *
 * This method splits the target string into its components using specific delimiters and assigns the values to the provided references.
 *
 * @param extrTarget The target string to parse.
 * @param nickname The extracted nickname.
 * @param username The extracted username.
 * @param hostname The extracted hostname.
 * @param serverName The extracted server name.
 */
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

/**
 * @brief Assigns user Log::INFOrmation by splitting the target string using the specified splitters.
 *
 * This method recursively splits the target string using the provided delimiters and assigns the values to the 
 * provided references.
 *
 * @param extrTarget The target string to split.
 * @param splitter The current splitter character.
 * @param splittersEnd The end iterator for the splitters.
 * @param nickname The extracted nickname.
 * @param username The extracted username.
 * @param hostname The extracted hostname.
 * @param serverName The extracted server name.
 */
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