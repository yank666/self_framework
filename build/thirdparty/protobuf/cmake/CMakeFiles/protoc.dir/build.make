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
include thirdparty/protobuf/cmake/CMakeFiles/protoc.dir/depend.make

# Include the progress variables for this target.
include thirdparty/protobuf/cmake/CMakeFiles/protoc.dir/progress.make

# Include the compile flags for this target's objects.
include thirdparty/protobuf/cmake/CMakeFiles/protoc.dir/flags.make

thirdparty/protobuf/cmake/CMakeFiles/protoc.dir/__/src/google/protobuf/compiler/main.cc.o: thirdparty/protobuf/cmake/CMakeFiles/protoc.dir/flags.make
thirdparty/protobuf/cmake/CMakeFiles/protoc.dir/__/src/google/protobuf/compiler/main.cc.o: ../thirdparty/protobuf/src/google/protobuf/compiler/main.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/yankai.yan/workbase/codeLib/refactor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object thirdparty/protobuf/cmake/CMakeFiles/protoc.dir/__/src/google/protobuf/compiler/main.cc.o"
	cd /home/yankai.yan/workbase/codeLib/refactor/build/thirdparty/protobuf/cmake && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/protoc.dir/__/src/google/protobuf/compiler/main.cc.o -c /home/yankai.yan/workbase/codeLib/refactor/thirdparty/protobuf/src/google/protobuf/compiler/main.cc

thirdparty/protobuf/cmake/CMakeFiles/protoc.dir/__/src/google/protobuf/compiler/main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/protoc.dir/__/src/google/protobuf/compiler/main.cc.i"
	cd /home/yankai.yan/workbase/codeLib/refactor/build/thirdparty/protobuf/cmake && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/yankai.yan/workbase/codeLib/refactor/thirdparty/protobuf/src/google/protobuf/compiler/main.cc > CMakeFiles/protoc.dir/__/src/google/protobuf/compiler/main.cc.i

thirdparty/protobuf/cmake/CMakeFiles/protoc.dir/__/src/google/protobuf/compiler/main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/protoc.dir/__/src/google/protobuf/compiler/main.cc.s"
	cd /home/yankai.yan/workbase/codeLib/refactor/build/thirdparty/protobuf/cmake && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/yankai.yan/workbase/codeLib/refactor/thirdparty/protobuf/src/google/protobuf/compiler/main.cc -o CMakeFiles/protoc.dir/__/src/google/protobuf/compiler/main.cc.s

# Object files for target protoc
protoc_OBJECTS = \
"CMakeFiles/protoc.dir/__/src/google/protobuf/compiler/main.cc.o"

# External object files for target protoc
protoc_EXTERNAL_OBJECTS =

thirdparty/protobuf/cmake/protoc-3.8.0.0: thirdparty/protobuf/cmake/CMakeFiles/protoc.dir/__/src/google/protobuf/compiler/main.cc.o
thirdparty/protobuf/cmake/protoc-3.8.0.0: thirdparty/protobuf/cmake/CMakeFiles/protoc.dir/build.make
thirdparty/protobuf/cmake/protoc-3.8.0.0: thirdparty/protobuf/cmake/libprotocd.a
thirdparty/protobuf/cmake/protoc-3.8.0.0: thirdparty/protobuf/cmake/libprotobufd.a
thirdparty/protobuf/cmake/protoc-3.8.0.0: /usr/local/lib/libz.so
thirdparty/protobuf/cmake/protoc-3.8.0.0: thirdparty/protobuf/cmake/CMakeFiles/protoc.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/yankai.yan/workbase/codeLib/refactor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable protoc"
	cd /home/yankai.yan/workbase/codeLib/refactor/build/thirdparty/protobuf/cmake && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/protoc.dir/link.txt --verbose=$(VERBOSE)
	cd /home/yankai.yan/workbase/codeLib/refactor/build/thirdparty/protobuf/cmake && $(CMAKE_COMMAND) -E cmake_symlink_executable protoc-3.8.0.0 protoc

thirdparty/protobuf/cmake/protoc: thirdparty/protobuf/cmake/protoc-3.8.0.0


# Rule to build all files generated by this target.
thirdparty/protobuf/cmake/CMakeFiles/protoc.dir/build: thirdparty/protobuf/cmake/protoc

.PHONY : thirdparty/protobuf/cmake/CMakeFiles/protoc.dir/build

thirdparty/protobuf/cmake/CMakeFiles/protoc.dir/clean:
	cd /home/yankai.yan/workbase/codeLib/refactor/build/thirdparty/protobuf/cmake && $(CMAKE_COMMAND) -P CMakeFiles/protoc.dir/cmake_clean.cmake
.PHONY : thirdparty/protobuf/cmake/CMakeFiles/protoc.dir/clean

thirdparty/protobuf/cmake/CMakeFiles/protoc.dir/depend:
	cd /home/yankai.yan/workbase/codeLib/refactor/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/yankai.yan/workbase/codeLib/refactor /home/yankai.yan/workbase/codeLib/refactor/thirdparty/protobuf/cmake /home/yankai.yan/workbase/codeLib/refactor/build /home/yankai.yan/workbase/codeLib/refactor/build/thirdparty/protobuf/cmake /home/yankai.yan/workbase/codeLib/refactor/build/thirdparty/protobuf/cmake/CMakeFiles/protoc.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : thirdparty/protobuf/cmake/CMakeFiles/protoc.dir/depend

