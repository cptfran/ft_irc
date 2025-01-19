#include <iostream>
#include <sstream>
#include "core/Log.h"
#include "utils/Colors.h"

// Log levels
const std::string Log::INFO = Colors::LIGHT_YELLOW + std::string("[INFO] ") + Colors::RESET;
const std::string Log::DEBUG = Colors::LIGHT_MAGENTA + std::string("[DEBUG] ") + Colors::RESET;
const std::string Log::ERROR = Colors::RED + std::string("[ERROR] ") + Colors::RESET;

// Log messages
const std::string Log::USAGE = Colors::LIGHT_CYAN + std::string("Usage: ./ircserv <port> <password>") + Colors::RESET;
const std::string Log::INVALID_PORT_INPUT = Colors::LIGHT_CYAN + std::string("Invalid port input.") + Colors::RESET;
const std::string Log::WRONG_PORT_RANGE = Colors::LIGHT_CYAN + std::string("Port is not within registered ports range. Try from 1024 to 49151.") + Colors::RESET;
const std::string Log::SOCKET_FAIL = Colors::LIGHT_CYAN + std::string("Socket initialization failed.") + Colors::RESET;
const std::string Log::BIND_FAIL = Colors::LIGHT_CYAN + std::string("Socket binding failed.") + Colors::RESET;
const std::string Log::LISTEN_FAIL = Colors::LIGHT_CYAN + std::string("Server listening failed.") + Colors::RESET;
const std::string Log::ACCEPT_FAIL = Colors::LIGHT_CYAN + std::string("Failed accepting incoming connection.") + Colors::RESET;
const std::string Log::NEW_CLIENT_START = Colors::LIGHT_CYAN + std::string("New client connecting...") + Colors::RESET;
const std::string Log::NEW_CLIENT_SUCCESS = Colors::LIGHT_CYAN + std::string("New client connected.") + Colors::RESET;
const std::string Log::CLIENT_REGISTER_SUCCESS = Colors::LIGHT_CYAN + std::string("Client registered successfully.") + Colors::RESET;
const std::string Log::SERVER_RUN = Colors::LIGHT_CYAN + std::string("Server is running and waiting for connections...") + Colors::RESET;
const std::string Log::HANDLE_CLIENT_FAIL = Colors::LIGHT_CYAN + std::string("Client disconnected or Log::ERROR occurred.") + Colors::RESET;
const std::string Log::CLIENT_CONNECTED = Colors::LIGHT_GREEN + std::string("Connected to the server.\n") + Colors::RESET;
const std::string Log::EMPTY_CLIENT_PROMPT = Colors::LIGHT_CYAN + std::string("Empty client prompt, nothing to handle.") + Colors::RESET;
const std::string Log::F_GETFL_FAIL = Colors::LIGHT_CYAN + std::string("Failed to fetch socket flags.") + Colors::RESET;
const std::string Log::F_SETFL_FAIL = Colors::LIGHT_CYAN + std::string("Failed to set socket to non-blocking mode.") + Colors::RESET;
const std::string Log::SETSOCKOPT_FAIL = Colors::LIGHT_CYAN + std::string("Failed to set socket options.") + Colors::RESET;
const std::string Log::CLIENT_DISCONNECTED = Colors::LIGHT_CYAN + std::string("Client disconnected.") + Colors::RESET;
const std::string Log::FIND_CHANNEL_FAIL_AFTER_ADD = Colors::LIGHT_CYAN + std::string("Failed to find the channel after adding it to the channels list.") + Colors::RESET;
const std::string Log::EJECT_CLIENT_FAIL = Colors::LIGHT_CYAN + std::string("Cannot kick user, user not found.") + Colors::RESET;

std::string Log::RPL_WRONG_NUM_OF_ARGS(const std::string& funcName)
{
    return Colors::LIGHT_CYAN + funcName + ": wrong number of arguments." + Colors::RESET;
}

/**
 * @brief Logs a message to the server console.
 *
 * This function logs a message to the server console based on the specified log level.
 * If the log level is "Log::INFO", the message is logged to the standard output (std::cout).
 * Otherwise, the message is logged to the standard Log::ERROR (std::cerr).
 *
 * @param logLevel The log level of the message (e.g., "Log::INFO", "Log::ERROR").
 * @param message The message to be logged.
 */
void Log::msgServer(const std::string& logLevel, const std::string& message)
{
    if (logLevel == Log::INFO)
    {
        std::cout << logLevel << message << std::endl;
    }
    else
    {
        std::cerr << logLevel + message << std::endl;
    }
}
