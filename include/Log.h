#pragma once

#include <iostream>
#include <vector>

#include "Colors.h"

// Log types:
#define INFO LIGHT_YELLOW "[INFO] " RESET
#define ERROR RED "[ERROR] " RESET

// Log messages:
#define USAGE LIGHT_CYAN "Usage: ./ircserv <port> <password>" RESET
#define INVALID_PORT_INPUT LIGHT_CYAN "Invalid port input." RESET
#define WRONG_PW LIGHT_CYAN "Password invalid." RESET
#define WRONG_PORT_RANGE LIGHT_CYAN "Port is not within registered ports range. Try from 1024 to 49151." RESET
#define SOCKET_FAIL LIGHT_CYAN "Socket initialization failed." RESET
#define BIND_FAIL LIGHT_CYAN "Socket binding failed." RESET
#define LISTEN_FAIL LIGHT_CYAN "Server listening failed." RESET
#define ACCEPT_FAIL LIGHT_CYAN "Failed accepting incoming connection." RESET
#define NEW_CLIENT_START LIGHT_CYAN "New client connecting..." RESET
#define NEW_CLIENT_SUCCESS LIGHT_CYAN "New client connected." RESET
// #define AUTHENTICATE_CLIENT_FAIL LIGHT_CYAN "Client authentication failed. Connection closed." RESET
#define AUTHENTICATE_CLIENT_SUCCESS LIGHT_CYAN "Client authenticated succesfully." RESET
#define SERVER_RUN LIGHT_CYAN "Server is running and waiting for connections..." RESET
#define HANDLE_CLIENT_FAIL LIGHT_CYAN "Client disconnected or error occured." RESET
#define CLIENT_CONNECTED LIGHT_GREEN "Connected to the server.\n" RESET
#define EMPTY_CLIENT_PROMPT LIGHT_CYAN "Empty client prompt, nothing to handle." RESET
#define F_GETFL_FAIL LIGHT_CYAN "Failed to fetch socket flags." RESET
#define F_SETFL_FAIL LIGHT_CYAN "Failed to set socket to non-blocking mode." RESET
#define SETSOCKOPT_FAIL LIGHT_CYAN "Failed to set socket options." RESET

class Log
{
public:
	template <typename Identifier>
	static void msgServer(const std::string& logLevel, const std::string& idType, Identifier id,
		const std::string& message);
	static void msgServer(const std::string& logLevel, const std::string& message);
};

#include "../src/Log.tpp"
