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
        include/commands/Invite.h
        include/commands/Join.h
        include/commands/Kick.h
        include/commands/Mode.h
        include/commands/Nick.h
        include/commands/Part.h
        include/commands/Pass.h
        include/commands/Ping.h
        include/commands/Topic.h
        include/commands/User.h
        include/commands/Who.h
        include/Channel.h
        include/Log.h
        include/Replier.h
        include/Server.h
        include/Utils.h
        src/Client.cpp
        src/ClientTranslator.cpp
        src/commands/Command.cpp
        src/commands/Cap.cpp
        src/commands/Invite.cpp
        src/commands/Join.cpp
        src/commands/Kick.cpp
        src/commands/Mode.cpp
        src/commands/Nick.cpp
        src/commands/Part.cpp
        src/commands/Pass.cpp
        src/commands/Ping.cpp
        src/commands/Topic.cpp
        src/commands/User.cpp
        src/commands/Who.cpp
        src/Channel.cpp
        src/Log.cpp
        src/main.cpp
        src/Replier.cpp
        src/Server.cpp
        src/Log.tpp
        src/Utils.cpp
)


message(STATUS "System: ${CMAKE_SYSTEM_NAME}")