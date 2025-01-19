#pragma once

#include <iostream>
#include <string>
#include "utils/Colors.h"

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
#define DEBUG_LOG(msg) Log::msgServer(Log::DEBUG, msg)
#else
#define DEBUG_LOG(msg) do {} while (0)
#endif

class Log
{
public:
    // Log levels:
    static const std::string INFO;
    static const std::string DEBUG;
    static const std::string ERROR;

    // Log messages:
    static const std::string USAGE;
    static const std::string INVALID_PORT_INPUT;
    static const std::string WRONG_PORT_RANGE;
    static const std::string SOCKET_FAIL;
    static const std::string BIND_FAIL;
    static const std::string LISTEN_FAIL;
    static const std::string ACCEPT_FAIL;
    static const std::string NEW_CLIENT_START;
    static const std::string NEW_CLIENT_SUCCESS;
    static const std::string CLIENT_REGISTER_SUCCESS;
    static const std::string SERVER_RUN;
    static const std::string HANDLE_CLIENT_FAIL;
    static const std::string CLIENT_CONNECTED;
    static const std::string EMPTY_CLIENT_PROMPT;
    static const std::string F_GETFL_FAIL;
    static const std::string F_SETFL_FAIL;
    static const std::string SETSOCKOPT_FAIL;
    static const std::string CLIENT_DISCONNECTED;
    static const std::string FIND_CHANNEL_FAIL_AFTER_ADD;
    static const std::string EJECT_CLIENT_FAIL;

    // Dynamic message:
    static std::string RPL_WRONG_NUM_OF_ARGS(const std::string& funcName);

    template <typename Identifier>
    static void msgServer(const std::string& logLevel, const std::string& idType, Identifier id,
        const std::string& message);
    static void msgServer(const std::string& logLevel, const std::string& message);

private:
    Log();                              
    ~Log();
    Log(const Log& toCopy);
    Log& operator=(const Log& toAssign);
};

#include "../../src/core/Log.tpp"
