cmake_minimum_required(VERSION 3.28)
project(ft_irc)

set(CMAKE_CXX_STANDARD 98)

include_directories(include)

add_executable(ircserv
        include/ClientTranslator.h
        include/Client.h
        include/Colors.h
        include/Log.h
        include/Server.h
        include/Utils.h
        src/Client.cpp
        src/ClientTranslator.cpp
        src/Log.cpp
        src/main.cpp
        src/Server.cpp
        src/Log.tpp
        src/Utils.cpp
)


message(STATUS "System: ${CMAKE_SYSTEM_NAME}")