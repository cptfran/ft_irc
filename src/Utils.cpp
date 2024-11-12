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
int Utils::stringToPositiveInt(const char *str)
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
