#include <utils/Utils.h>
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

std::string Utils::stringToUpper(const std::string& str) 
{
	std::string result = str;
	for (std::string::iterator it = result.begin(); it != result.end(); ++it) 
	{
		*it = toupper(*it);
	}

	return result;
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