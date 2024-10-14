cmake_minimum_required(VERSION 3.28)
project(ft_irc)

set(CMAKE_CXX_STANDARD 98)

include_directories(include)

add_executable(ircserv
        src/main.cpp
        include/Server.h
        include/Logging.h
        include/Colors.h
        src/Server.cpp
        src/Server.cpp
        src/Logging.cpp
        src/Server.cpp
)

message(STATUS "System: ${CMAKE_SYSTEM_NAME}")