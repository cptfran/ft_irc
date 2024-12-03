#pragma once

#include <iostream>
#include "Colors.h"

/**
 * @brief Enables or disables debug logging based on the DEBUGGING macro.
 *
 * To enable debug logging use 'make debug', it will set the DEBUGGING macro to '1' (implementation in the Makefile).
 *
 * If DEBUGGING is set to 1, DEBUG_LOG(msg) will log the message using Log::msgServer with DEBUG level.
 * If DEBUGGING is set to 0, DEBUG_LOG(msg) will do nothing.
 *
 * Usage:
 * @code
 * DEBUG_LOG("This is a debug message");
 * @endcode
 *
 * Define DEBUGGING if not already defined.
 */
#ifndef DEBUGGING
#define DEBUGGING 0
#endif
#if DEBUGGING
#define DEBUG_LOG(msg) Log::msgServer(DEBUG, msg)
#else
#define DEBUG_LOG(msg) do {} while (0)
#endif

// Log types:
#define INFO LIGHT_YELLOW "[INFO] " RESET
#define DEBUG LIGHT_MAGENTA "[DEBUG] " RESET
#define ERROR RED "[ERROR] " RESET

// Log messages:
#define USAGE LIGHT_CYAN "Usage: ./ircserv <port> <password>" RESET
#define INVALID_PORT_INPUT LIGHT_CYAN "Invalid port input." RESET
#define WRONG_PORT_RANGE LIGHT_CYAN "Port is not within registered ports range. Try from 1024 to 49151." RESET
#define SOCKET_FAIL LIGHT_CYAN "Socket initialization failed." RESET
#define BIND_FAIL LIGHT_CYAN "Socket binding failed." RESET
#define LISTEN_FAIL LIGHT_CYAN "Server listening failed." RESET
#define ACCEPT_FAIL LIGHT_CYAN "Failed accepting incoming connection." RESET
#define NEW_CLIENT_START LIGHT_CYAN "New client connecting..." RESET
#define NEW_CLIENT_SUCCESS LIGHT_CYAN "New client connected." RESET
// #define AUTHENTICATE_CLIENT_FAIL LIGHT_CYAN "Client authentication failed. Connection closed." RESET
#define CLIENT_REGISTER_SUCCESS LIGHT_CYAN "Client registered succesfully." RESET
#define SERVER_RUN LIGHT_CYAN "Server is running and waiting for connections..." RESET
#define HANDLE_CLIENT_FAIL LIGHT_CYAN "Client disconnected or error occured." RESET
#define CLIENT_CONNECTED LIGHT_GREEN "Connected to the server.\n" RESET
#define EMPTY_CLIENT_PROMPT LIGHT_CYAN "Empty client prompt, nothing to handle." RESET
#define F_GETFL_FAIL LIGHT_CYAN "Failed to fetch socket flags." RESET
#define F_SETFL_FAIL LIGHT_CYAN "Failed to set socket to non-blocking mode." RESET
#define SETSOCKOPT_FAIL LIGHT_CYAN "Failed to set socket options." RESET
#define WRONG_PASSWORD LIGHT_CYAN "Client entered wrong password." RESET
#define RPL_WRONG_NUM_OF_ARGS(funcName) (std::string(LIGHT_CYAN) + (funcName) + ": wrong number of arguments." + RESET)
#define CLIENT_DISCONNECTED LIGHT_CYAN "Client disconnected." RESET
#define PASSWORD_EMPTY LIGHT_CYAN "Password is empty." RESET
#define FIND_CHANNEL_FAIL_AFTER_ADD LIGHT_CYAN "Failed to find the channel after adding it to the channels list." RESET

class Log
{
public:
	template <typename Identifier>
	static void msgServer(const std::string& logLevel, const std::string& idType, Identifier id,
		const std::string& message);
	static void msgServer(const std::string& logLevel, const std::string& message);
};

#include "../src/Log.tpp"
