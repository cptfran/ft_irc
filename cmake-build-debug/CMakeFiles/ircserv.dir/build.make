# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/c/Users/Szymon/CLionProjects/ft_irc

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/c/Users/Szymon/CLionProjects/ft_irc/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/ircserv.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/ircserv.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/ircserv.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ircserv.dir/flags.make

CMakeFiles/ircserv.dir/src/main.cpp.o: CMakeFiles/ircserv.dir/flags.make
CMakeFiles/ircserv.dir/src/main.cpp.o: /mnt/c/Users/Szymon/CLionProjects/ft_irc/src/main.cpp
CMakeFiles/ircserv.dir/src/main.cpp.o: CMakeFiles/ircserv.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/c/Users/Szymon/CLionProjects/ft_irc/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/ircserv.dir/src/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/ircserv.dir/src/main.cpp.o -MF CMakeFiles/ircserv.dir/src/main.cpp.o.d -o CMakeFiles/ircserv.dir/src/main.cpp.o -c /mnt/c/Users/Szymon/CLionProjects/ft_irc/src/main.cpp

CMakeFiles/ircserv.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/ircserv.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/c/Users/Szymon/CLionProjects/ft_irc/src/main.cpp > CMakeFiles/ircserv.dir/src/main.cpp.i

CMakeFiles/ircserv.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/ircserv.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/c/Users/Szymon/CLionProjects/ft_irc/src/main.cpp -o CMakeFiles/ircserv.dir/src/main.cpp.s

CMakeFiles/ircserv.dir/src/Server.cpp.o: CMakeFiles/ircserv.dir/flags.make
CMakeFiles/ircserv.dir/src/Server.cpp.o: /mnt/c/Users/Szymon/CLionProjects/ft_irc/src/Server.cpp
CMakeFiles/ircserv.dir/src/Server.cpp.o: CMakeFiles/ircserv.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/c/Users/Szymon/CLionProjects/ft_irc/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/ircserv.dir/src/Server.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/ircserv.dir/src/Server.cpp.o -MF CMakeFiles/ircserv.dir/src/Server.cpp.o.d -o CMakeFiles/ircserv.dir/src/Server.cpp.o -c /mnt/c/Users/Szymon/CLionProjects/ft_irc/src/Server.cpp

CMakeFiles/ircserv.dir/src/Server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/ircserv.dir/src/Server.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/c/Users/Szymon/CLionProjects/ft_irc/src/Server.cpp > CMakeFiles/ircserv.dir/src/Server.cpp.i

CMakeFiles/ircserv.dir/src/Server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/ircserv.dir/src/Server.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/c/Users/Szymon/CLionProjects/ft_irc/src/Server.cpp -o CMakeFiles/ircserv.dir/src/Server.cpp.s

CMakeFiles/ircserv.dir/src/Logging.cpp.o: CMakeFiles/ircserv.dir/flags.make
CMakeFiles/ircserv.dir/src/Logging.cpp.o: /mnt/c/Users/Szymon/CLionProjects/ft_irc/src/Logging.cpp
CMakeFiles/ircserv.dir/src/Logging.cpp.o: CMakeFiles/ircserv.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/c/Users/Szymon/CLionProjects/ft_irc/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/ircserv.dir/src/Logging.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/ircserv.dir/src/Logging.cpp.o -MF CMakeFiles/ircserv.dir/src/Logging.cpp.o.d -o CMakeFiles/ircserv.dir/src/Logging.cpp.o -c /mnt/c/Users/Szymon/CLionProjects/ft_irc/src/Logging.cpp

CMakeFiles/ircserv.dir/src/Logging.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/ircserv.dir/src/Logging.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/c/Users/Szymon/CLionProjects/ft_irc/src/Logging.cpp > CMakeFiles/ircserv.dir/src/Logging.cpp.i

CMakeFiles/ircserv.dir/src/Logging.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/ircserv.dir/src/Logging.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/c/Users/Szymon/CLionProjects/ft_irc/src/Logging.cpp -o CMakeFiles/ircserv.dir/src/Logging.cpp.s

CMakeFiles/ircserv.dir/src/Client.cpp.o: CMakeFiles/ircserv.dir/flags.make
CMakeFiles/ircserv.dir/src/Client.cpp.o: /mnt/c/Users/Szymon/CLionProjects/ft_irc/src/Client.cpp
CMakeFiles/ircserv.dir/src/Client.cpp.o: CMakeFiles/ircserv.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/c/Users/Szymon/CLionProjects/ft_irc/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/ircserv.dir/src/Client.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/ircserv.dir/src/Client.cpp.o -MF CMakeFiles/ircserv.dir/src/Client.cpp.o.d -o CMakeFiles/ircserv.dir/src/Client.cpp.o -c /mnt/c/Users/Szymon/CLionProjects/ft_irc/src/Client.cpp

CMakeFiles/ircserv.dir/src/Client.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/ircserv.dir/src/Client.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/c/Users/Szymon/CLionProjects/ft_irc/src/Client.cpp > CMakeFiles/ircserv.dir/src/Client.cpp.i

CMakeFiles/ircserv.dir/src/Client.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/ircserv.dir/src/Client.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/c/Users/Szymon/CLionProjects/ft_irc/src/Client.cpp -o CMakeFiles/ircserv.dir/src/Client.cpp.s

CMakeFiles/ircserv.dir/src/Parser.cpp.o: CMakeFiles/ircserv.dir/flags.make
CMakeFiles/ircserv.dir/src/Parser.cpp.o: /mnt/c/Users/Szymon/CLionProjects/ft_irc/src/Parser.cpp
CMakeFiles/ircserv.dir/src/Parser.cpp.o: CMakeFiles/ircserv.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/mnt/c/Users/Szymon/CLionProjects/ft_irc/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/ircserv.dir/src/Parser.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/ircserv.dir/src/Parser.cpp.o -MF CMakeFiles/ircserv.dir/src/Parser.cpp.o.d -o CMakeFiles/ircserv.dir/src/Parser.cpp.o -c /mnt/c/Users/Szymon/CLionProjects/ft_irc/src/Parser.cpp

CMakeFiles/ircserv.dir/src/Parser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/ircserv.dir/src/Parser.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/c/Users/Szymon/CLionProjects/ft_irc/src/Parser.cpp > CMakeFiles/ircserv.dir/src/Parser.cpp.i

CMakeFiles/ircserv.dir/src/Parser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/ircserv.dir/src/Parser.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/c/Users/Szymon/CLionProjects/ft_irc/src/Parser.cpp -o CMakeFiles/ircserv.dir/src/Parser.cpp.s

# Object files for target ircserv
ircserv_OBJECTS = \
"CMakeFiles/ircserv.dir/src/main.cpp.o" \
"CMakeFiles/ircserv.dir/src/Server.cpp.o" \
"CMakeFiles/ircserv.dir/src/Logging.cpp.o" \
"CMakeFiles/ircserv.dir/src/Client.cpp.o" \
"CMakeFiles/ircserv.dir/src/Parser.cpp.o"

# External object files for target ircserv
ircserv_EXTERNAL_OBJECTS =

ircserv: CMakeFiles/ircserv.dir/src/main.cpp.o
ircserv: CMakeFiles/ircserv.dir/src/Server.cpp.o
ircserv: CMakeFiles/ircserv.dir/src/Logging.cpp.o
ircserv: CMakeFiles/ircserv.dir/src/Client.cpp.o
ircserv: CMakeFiles/ircserv.dir/src/Parser.cpp.o
ircserv: CMakeFiles/ircserv.dir/build.make
ircserv: CMakeFiles/ircserv.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/mnt/c/Users/Szymon/CLionProjects/ft_irc/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX executable ircserv"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ircserv.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ircserv.dir/build: ircserv
.PHONY : CMakeFiles/ircserv.dir/build

CMakeFiles/ircserv.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ircserv.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ircserv.dir/clean

CMakeFiles/ircserv.dir/depend:
	cd /mnt/c/Users/Szymon/CLionProjects/ft_irc/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/c/Users/Szymon/CLionProjects/ft_irc /mnt/c/Users/Szymon/CLionProjects/ft_irc /mnt/c/Users/Szymon/CLionProjects/ft_irc/cmake-build-debug /mnt/c/Users/Szymon/CLionProjects/ft_irc/cmake-build-debug /mnt/c/Users/Szymon/CLionProjects/ft_irc/cmake-build-debug/CMakeFiles/ircserv.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/ircserv.dir/depend

