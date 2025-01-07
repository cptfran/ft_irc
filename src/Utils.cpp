#include "../include/Utils.h"
#include <sstream>

/**
 * @brief Converts a C-style string to a positive integer.
 *
 * This function takes a C-style string as input and attempts to convert it to an integer.
 * If the conversion fails or the resulting integer is not positive, the function returns 0.
 * Otherwise, it returns the converted positive integer.
 *
 * @param str The C-style string to be converted to a positive integer.
 * @return The converted positive integer, or 0 if the conversion fails or the integer is not positive.
 */
int Utils::cStringToPositiveInt(const char *str)
{
	std::stringstream ss(str);
	int num;
	ss >> num;
	if (ss.fail() || num <= 0)
	{
		return 0;
	}
	return num;
}

std::string Utils::intToString(const int value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

std::vector<std::string> Utils::splitStringByComma(const std::string& str)
{
	std::vector<std::string> tokens;
	std::string token;
	std::stringstream ss(str);

	while (std::getline(ss, token, ','))
	{
		tokens.push_back(token);
	}

	return tokens;
}

std::string Utils::sanitizeColonMessage(const std::string& message)
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