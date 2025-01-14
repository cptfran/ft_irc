cmake_minimum_required(VERSION 3.28)
project(ft_irc)

set(CMAKE_CXX_STANDARD 98)

include_directories(include)

add_executable(ircserv
        include/client/ClientTranslator.h
        include/client/Client.h
        include/commands/Cap.h
        include/commands/Command.h
        include/commands/Invite.h
        include/commands/Join.h
        include/commands/Kick.h
        include/commands/Nick.h
        include/commands/Pass.h
        include/commands/Ping.h
        include/commands/Privmsg.h
        include/commands/Topic.h
        include/commands/User.h
        include/commands/Who.h
        include/log/Log.h
        include/replier/Replier.h
        include/server/Server.h
        include/utils/Colors.h
        include/utils/Utils.h
        src/client/Client.cpp
        src/client/ClientTranslator.cpp
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
        src/commands/Privmsg.cpp
        src/commands/Topic.cpp
        src/commands/User.cpp
        src/commands/Who.cpp
        src/channel/Channel.cpp
        src/log/Log.cpp
        src/log/Log.tpp
        src/main.cpp
        src/replier/Replier.cpp
        src/server/Server.cpp
        src/utils/Utils.cpp
        src/utils/Utils.tpp
 "src/server/Log.cpp" "include/server/Log.h")

message(STATUS "System: ${CMAKE_SYSTEM_NAME}")