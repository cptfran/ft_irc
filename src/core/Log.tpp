#pragma once

/**
 * @brief Logs a message to the server console with an identifier.
 *
 * This template function logs a message to the server console based on the specified log level.
 * It includes an identifier type and identifier in the log message for better context.
 * If the log level is "Log::INFO", the message is logged to the standard output (std::cout).
 * Otherwise, the message is logged to the standard Log::ERROR (std::cerr).
 *
 * @tparam Identifier The type of the identifier (e.g., int, std::string).
 * @param logLevel The log level of the message (e.g., "Log::INFO", "Log::ERROR").
 * @param idType The type of the identifier (e.g., "CLIENT", "SERVER").
 * @param id The identifier value.
 * @param message The message to be logged.
 */
template <typename Identifier>
void Log::msgServer(const std::string& logLevel, const std::string& idType, Identifier id, const std::string& message)
{
	if (logLevel == Log::INFO)
	{
		std::cout << logLevel << "[" << idType << " " << id << "] " << message << std::endl;
	}
	else
	{
		std::cerr << logLevel << "[" << idType << " " << id << "] " << message << std::endl;
	}
}
