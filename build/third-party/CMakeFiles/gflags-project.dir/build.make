# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/femu/ZNSBench

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/femu/ZNSBench/build

# Utility rule file for gflags-project.

# Include the progress variables for this target.
include third-party/CMakeFiles/gflags-project.dir/progress.make

third-party/CMakeFiles/gflags-project: third-party/CMakeFiles/gflags-project-complete


third-party/CMakeFiles/gflags-project-complete: third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-install
third-party/CMakeFiles/gflags-project-complete: third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-mkdir
third-party/CMakeFiles/gflags-project-complete: third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-download
third-party/CMakeFiles/gflags-project-complete: third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-update
third-party/CMakeFiles/gflags-project-complete: third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-patch
third-party/CMakeFiles/gflags-project-complete: third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-configure
third-party/CMakeFiles/gflags-project-complete: third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-build
third-party/CMakeFiles/gflags-project-complete: third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-install
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/femu/ZNSBench/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Completed 'gflags-project'"
	cd /home/femu/ZNSBench/build/third-party && /usr/bin/cmake -E make_directory /home/femu/ZNSBench/build/third-party/CMakeFiles
	cd /home/femu/ZNSBench/build/third-party && /usr/bin/cmake -E touch /home/femu/ZNSBench/build/third-party/CMakeFiles/gflags-project-complete
	cd /home/femu/ZNSBench/build/third-party && /usr/bin/cmake -E touch /home/femu/ZNSBench/build/third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-done

third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-install: third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-build
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/femu/ZNSBench/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Performing install step for 'gflags-project'"
	cd /home/femu/ZNSBench/build/third-party/gflags-project-prefix/src/gflags-project-build && $(MAKE) install
	cd /home/femu/ZNSBench/build/third-party/gflags-project-prefix/src/gflags-project-build && /usr/bin/cmake -E touch /home/femu/ZNSBench/build/third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-install

third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-mkdir:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/femu/ZNSBench/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Creating directories for 'gflags-project'"
	cd /home/femu/ZNSBench/build/third-party && /usr/bin/cmake -E make_directory /home/femu/ZNSBench/third-party/gflags
	cd /home/femu/ZNSBench/build/third-party && /usr/bin/cmake -E make_directory /home/femu/ZNSBench/build/third-party/gflags-project-prefix/src/gflags-project-build
	cd /home/femu/ZNSBench/build/third-party && /usr/bin/cmake -E make_directory /home/femu/ZNSBench/build/third-party/gflags-project-prefix
	cd /home/femu/ZNSBench/build/third-party && /usr/bin/cmake -E make_directory /home/femu/ZNSBench/build/third-party/gflags-project-prefix/tmp
	cd /home/femu/ZNSBench/build/third-party && /usr/bin/cmake -E make_directory /home/femu/ZNSBench/build/third-party/gflags-project-prefix/src/gflags-project-stamp
	cd /home/femu/ZNSBench/build/third-party && /usr/bin/cmake -E make_directory /home/femu/ZNSBench/build/third-party/gflags-project-prefix/src
	cd /home/femu/ZNSBench/build/third-party && /usr/bin/cmake -E make_directory /home/femu/ZNSBench/build/third-party/gflags-project-prefix/src/gflags-project-stamp
	cd /home/femu/ZNSBench/build/third-party && /usr/bin/cmake -E touch /home/femu/ZNSBench/build/third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-mkdir

third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-download: third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-mkdir
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/femu/ZNSBench/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "No download step for 'gflags-project'"
	cd /home/femu/ZNSBench/build/third-party && /usr/bin/cmake -E echo_append
	cd /home/femu/ZNSBench/build/third-party && /usr/bin/cmake -E touch /home/femu/ZNSBench/build/third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-download

third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-update: third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-download
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/femu/ZNSBench/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "No update step for 'gflags-project'"
	cd /home/femu/ZNSBench/build/third-party && /usr/bin/cmake -E echo_append
	cd /home/femu/ZNSBench/build/third-party && /usr/bin/cmake -E touch /home/femu/ZNSBench/build/third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-update

third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-patch: third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-download
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/femu/ZNSBench/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "No patch step for 'gflags-project'"
	cd /home/femu/ZNSBench/build/third-party && /usr/bin/cmake -E echo_append
	cd /home/femu/ZNSBench/build/third-party && /usr/bin/cmake -E touch /home/femu/ZNSBench/build/third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-patch

third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-configure: third-party/gflags-project-prefix/tmp/gflags-project-cfgcmd.txt
third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-configure: third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-update
third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-configure: third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-patch
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/femu/ZNSBench/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Performing configure step for 'gflags-project'"
	cd /home/femu/ZNSBench/build/third-party/gflags-project-prefix/src/gflags-project-build && cmake /home/femu/ZNSBench/third-party/gflags -DCMAKE_INSTALL_PREFIX=/home/femu/ZNSBench/build -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS=-fPIC -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_VERBOSE_MAKEFILE=FALSE
	cd /home/femu/ZNSBench/build/third-party/gflags-project-prefix/src/gflags-project-build && /usr/bin/cmake -E touch /home/femu/ZNSBench/build/third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-configure

third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-build: third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-configure
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/femu/ZNSBench/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Performing build step for 'gflags-project'"
	cd /home/femu/ZNSBench/build/third-party/gflags-project-prefix/src/gflags-project-build && $(MAKE)
	cd /home/femu/ZNSBench/build/third-party/gflags-project-prefix/src/gflags-project-build && /usr/bin/cmake -E touch /home/femu/ZNSBench/build/third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-build

gflags-project: third-party/CMakeFiles/gflags-project
gflags-project: third-party/CMakeFiles/gflags-project-complete
gflags-project: third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-install
gflags-project: third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-mkdir
gflags-project: third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-download
gflags-project: third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-update
gflags-project: third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-patch
gflags-project: third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-configure
gflags-project: third-party/gflags-project-prefix/src/gflags-project-stamp/gflags-project-build
gflags-project: third-party/CMakeFiles/gflags-project.dir/build.make

.PHONY : gflags-project

# Rule to build all files generated by this target.
third-party/CMakeFiles/gflags-project.dir/build: gflags-project

.PHONY : third-party/CMakeFiles/gflags-project.dir/build

third-party/CMakeFiles/gflags-project.dir/clean:
	cd /home/femu/ZNSBench/build/third-party && $(CMAKE_COMMAND) -P CMakeFiles/gflags-project.dir/cmake_clean.cmake
.PHONY : third-party/CMakeFiles/gflags-project.dir/clean

third-party/CMakeFiles/gflags-project.dir/depend:
	cd /home/femu/ZNSBench/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/femu/ZNSBench /home/femu/ZNSBench/third-party /home/femu/ZNSBench/build /home/femu/ZNSBench/build/third-party /home/femu/ZNSBench/build/third-party/CMakeFiles/gflags-project.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : third-party/CMakeFiles/gflags-project.dir/depend
