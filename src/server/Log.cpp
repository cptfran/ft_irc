#include <iostream>
#include <sstream>
#include "client/Client.h"
#include "server/Log.h"
#include <sys/socket.h>

/**
 * @brief Logs a message to the server console.
 *
 * This function logs a message to the server console based on the specified log level.
 * If the log level is "INFO", the message is logged to the standard output (std::cout).
 * Otherwise, the message is logged to the standard error (std::cerr).
 *
 * @param logLevel The log level of the message (e.g., "INFO", "ERROR").
 * @param message The message to be logged.
 */
void Log::msgServer(const std::string& logLevel, const std::string& message)
{
	if (logLevel == INFO)
	{
		std::cout << logLevel << message << std::endl;
	}
	else
	{
		std::cerr << logLevel + message << std::endl;
	}
}