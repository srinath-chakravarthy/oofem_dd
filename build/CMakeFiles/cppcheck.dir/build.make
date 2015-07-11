# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.2

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /opt/neu/cmake/bin/cmake

# The command to remove a file.
RM = /opt/neu/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /scratch/oofem_dd.git

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /scratch/oofem_dd.git/build

# Utility rule file for cppcheck.

# Include the progress variables for this target.
include CMakeFiles/cppcheck.dir/progress.make

cppcheck: CMakeFiles/cppcheck.dir/build.make
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Running cppcheck on entire source"
	cppcheck --xml --template=gcc --enable=all -I/scratch/oofem_dd.git/build/ -I/scratch/oofem_dd.git/src/oofemlib/ -I/scratch/oofem_dd.git/src/sm/ -I/scratch/oofem_dd.git/src/fm/ -I/scratch/oofem_dd.git/src/tm/ -DDEBUG /scratch/oofem_dd.git/src/oofemlib/ /scratch/oofem_dd.git/src/sm/ /scratch/oofem_dd.git/src/fm/ /scratch/oofem_dd.git/src/tm/ /scratch/oofem_dd.git/src/main/ 2> cppcheck.xml
.PHONY : cppcheck

# Rule to build all files generated by this target.
CMakeFiles/cppcheck.dir/build: cppcheck

.PHONY : CMakeFiles/cppcheck.dir/build

CMakeFiles/cppcheck.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/cppcheck.dir/cmake_clean.cmake
.PHONY : CMakeFiles/cppcheck.dir/clean

CMakeFiles/cppcheck.dir/depend:
	cd /scratch/oofem_dd.git/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /scratch/oofem_dd.git /scratch/oofem_dd.git /scratch/oofem_dd.git/build /scratch/oofem_dd.git/build /scratch/oofem_dd.git/build/CMakeFiles/cppcheck.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/cppcheck.dir/depend
