# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/yankai.yan/workbase/codeLib/refactor

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/yankai.yan/workbase/codeLib/refactor/build

# Include any dependencies generated for this target.
include thirdparty/gtest/googlemock/CMakeFiles/gmock.dir/depend.make

# Include the progress variables for this target.
include thirdparty/gtest/googlemock/CMakeFiles/gmock.dir/progress.make

# Include the compile flags for this target's objects.
include thirdparty/gtest/googlemock/CMakeFiles/gmock.dir/flags.make

thirdparty/gtest/googlemock/CMakeFiles/gmock.dir/src/gmock-all.cc.o: thirdparty/gtest/googlemock/CMakeFiles/gmock.dir/flags.make
thirdparty/gtest/googlemock/CMakeFiles/gmock.dir/src/gmock-all.cc.o: ../thirdparty/gtest/googlemock/src/gmock-all.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/yankai.yan/workbase/codeLib/refactor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object thirdparty/gtest/googlemock/CMakeFiles/gmock.dir/src/gmock-all.cc.o"
	cd /home/yankai.yan/workbase/codeLib/refactor/build/thirdparty/gtest/googlemock && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gmock.dir/src/gmock-all.cc.o -c /home/yankai.yan/workbase/codeLib/refactor/thirdparty/gtest/googlemock/src/gmock-all.cc

thirdparty/gtest/googlemock/CMakeFiles/gmock.dir/src/gmock-all.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gmock.dir/src/gmock-all.cc.i"
	cd /home/yankai.yan/workbase/codeLib/refactor/build/thirdparty/gtest/googlemock && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/yankai.yan/workbase/codeLib/refactor/thirdparty/gtest/googlemock/src/gmock-all.cc > CMakeFiles/gmock.dir/src/gmock-all.cc.i

thirdparty/gtest/googlemock/CMakeFiles/gmock.dir/src/gmock-all.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gmock.dir/src/gmock-all.cc.s"
	cd /home/yankai.yan/workbase/codeLib/refactor/build/thirdparty/gtest/googlemock && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/yankai.yan/workbase/codeLib/refactor/thirdparty/gtest/googlemock/src/gmock-all.cc -o CMakeFiles/gmock.dir/src/gmock-all.cc.s

# Object files for target gmock
gmock_OBJECTS = \
"CMakeFiles/gmock.dir/src/gmock-all.cc.o"

# External object files for target gmock
gmock_EXTERNAL_OBJECTS =

lib/libgmockd.a: thirdparty/gtest/googlemock/CMakeFiles/gmock.dir/src/gmock-all.cc.o
lib/libgmockd.a: thirdparty/gtest/googlemock/CMakeFiles/gmock.dir/build.make
lib/libgmockd.a: thirdparty/gtest/googlemock/CMakeFiles/gmock.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/yankai.yan/workbase/codeLib/refactor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library ../../../lib/libgmockd.a"
	cd /home/yankai.yan/workbase/codeLib/refactor/build/thirdparty/gtest/googlemock && $(CMAKE_COMMAND) -P CMakeFiles/gmock.dir/cmake_clean_target.cmake
	cd /home/yankai.yan/workbase/codeLib/refactor/build/thirdparty/gtest/googlemock && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gmock.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
thirdparty/gtest/googlemock/CMakeFiles/gmock.dir/build: lib/libgmockd.a

.PHONY : thirdparty/gtest/googlemock/CMakeFiles/gmock.dir/build

thirdparty/gtest/googlemock/CMakeFiles/gmock.dir/clean:
	cd /home/yankai.yan/workbase/codeLib/refactor/build/thirdparty/gtest/googlemock && $(CMAKE_COMMAND) -P CMakeFiles/gmock.dir/cmake_clean.cmake
.PHONY : thirdparty/gtest/googlemock/CMakeFiles/gmock.dir/clean

thirdparty/gtest/googlemock/CMakeFiles/gmock.dir/depend:
	cd /home/yankai.yan/workbase/codeLib/refactor/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/yankai.yan/workbase/codeLib/refactor /home/yankai.yan/workbase/codeLib/refactor/thirdparty/gtest/googlemock /home/yankai.yan/workbase/codeLib/refactor/build /home/yankai.yan/workbase/codeLib/refactor/build/thirdparty/gtest/googlemock /home/yankai.yan/workbase/codeLib/refactor/build/thirdparty/gtest/googlemock/CMakeFiles/gmock.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : thirdparty/gtest/googlemock/CMakeFiles/gmock.dir/depend

