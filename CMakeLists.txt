cmake_minimum_required(VERSION 3.28)
project(ft_irc)

set(CMAKE_CXX_STANDARD 98)

include_directories(include)

add_executable(ircserv
        include/ClientTranslator.h
        include/Client.h
        include/Colors.h
        include/commands/Cap.h
        include/commands/Command.h
        include/commands/Join.h
        include/commands/Nick.h
        include/commands/Pass.h
        include/commands/Ping.h
        include/commands/User.h
        include/Log.h
        include/Server.h
        include/Utils.h
        src/Client.cpp
        src/ClientTranslator.cpp
        src/commands/Command.cpp
        src/commands/Cap.cpp
        src/commands/Join.cpp
        src/commands/Nick.cpp
        src/commands/Pass.cpp
        src/commands/Ping.cpp
        src/commands/User.cpp
        src/Log.cpp
        src/main.cpp
        src/Server.cpp
        src/Log.tpp
        src/Utils.cpp
)


message(STATUS "System: ${CMAKE_SYSTEM_NAME}")