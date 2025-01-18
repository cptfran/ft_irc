cmake_minimum_required(VERSION 3.28)
project(ft_irc)

set(CMAKE_CXX_STANDARD 98)

include_directories(include)

add_executable(ircserv
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
        include/communication/ClientTranslator.h
        include/communication/Replier.h
        include/core/Log.h
        include/core/Server.h
        include/data/Channel.h
        include/data/Client.h
        include/manager/ChannelManager.h
        include/manager/ClientManager.h
        include/manager/CommandManager.h
        include/manager/ConfigManager.h
        include/manager/ConnectionManager.h
        include/manager/Manager.h
        include/utils/Colors.h
        include/utils/Utils.h
        src/commands/Cap.cpp
        src/commands/Command.cpp
        src/commands/Invite.cpp
        src/commands/Join.cpp
        src/commands/Kick.cpp
        src/commands/Nick.cpp
        src/commands/Pass.cpp
        src/commands/Ping.cpp
        src/commands/Privmsg.cpp
        src/commands/Topic.cpp
        src/commands/User.cpp
        src/commands/Who.cpp
        src/communication/ClientTranslator.cpp
        src/communication/Replier.cpp
        src/core/Log.cpp
        src/core/Server.cpp
        src/data/Channel.cpp
        src/data/Client.cpp
        src/Main.cpp
        src/manager/ChannelManager.cpp
        src/manager/ClientManager.cpp
        src/manager/CommandManager.cpp
        src/manager/ConfigManager.cpp
        src/manager/ConnectionManager.cpp
        src/manager/Manager.cpp
        src/utils/Utils.cpp
 )

message(STATUS "System: ${CMAKE_SYSTEM_NAME}")