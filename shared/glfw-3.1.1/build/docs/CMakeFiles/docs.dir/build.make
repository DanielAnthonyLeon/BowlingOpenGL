# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /u5/daleon/cs488/cs488/shared/glfw-3.1.1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /u5/daleon/cs488/cs488/shared/glfw-3.1.1/build

# Utility rule file for docs.

# Include the progress variables for this target.
include docs/CMakeFiles/docs.dir/progress.make

docs/CMakeFiles/docs:
	$(CMAKE_COMMAND) -E cmake_progress_report /u5/daleon/cs488/cs488/shared/glfw-3.1.1/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating HTML documentation"
	cd /u5/daleon/cs488/cs488/shared/glfw-3.1.1/build/docs && /usr/bin/doxygen

docs: docs/CMakeFiles/docs
docs: docs/CMakeFiles/docs.dir/build.make
.PHONY : docs

# Rule to build all files generated by this target.
docs/CMakeFiles/docs.dir/build: docs
.PHONY : docs/CMakeFiles/docs.dir/build

docs/CMakeFiles/docs.dir/clean:
	cd /u5/daleon/cs488/cs488/shared/glfw-3.1.1/build/docs && $(CMAKE_COMMAND) -P CMakeFiles/docs.dir/cmake_clean.cmake
.PHONY : docs/CMakeFiles/docs.dir/clean

docs/CMakeFiles/docs.dir/depend:
	cd /u5/daleon/cs488/cs488/shared/glfw-3.1.1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /u5/daleon/cs488/cs488/shared/glfw-3.1.1 /u5/daleon/cs488/cs488/shared/glfw-3.1.1/docs /u5/daleon/cs488/cs488/shared/glfw-3.1.1/build /u5/daleon/cs488/cs488/shared/glfw-3.1.1/build/docs /u5/daleon/cs488/cs488/shared/glfw-3.1.1/build/docs/CMakeFiles/docs.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : docs/CMakeFiles/docs.dir/depend

