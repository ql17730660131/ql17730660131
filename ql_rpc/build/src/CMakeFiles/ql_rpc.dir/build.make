# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.27

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
CMAKE_COMMAND = /usr/local/cmake/bin/cmake

# The command to remove a file.
RM = /usr/local/cmake/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /root/qianlan/project/ql_rpc

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/qianlan/project/ql_rpc/build

# Include any dependencies generated for this target.
include src/CMakeFiles/ql_rpc.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/CMakeFiles/ql_rpc.dir/compiler_depend.make

# Include the progress variables for this target.
include src/CMakeFiles/ql_rpc.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/ql_rpc.dir/flags.make

src/CMakeFiles/ql_rpc.dir/logger.cpp.o: src/CMakeFiles/ql_rpc.dir/flags.make
src/CMakeFiles/ql_rpc.dir/logger.cpp.o: /root/qianlan/project/ql_rpc/src/logger.cpp
src/CMakeFiles/ql_rpc.dir/logger.cpp.o: src/CMakeFiles/ql_rpc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/qianlan/project/ql_rpc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/ql_rpc.dir/logger.cpp.o"
	cd /root/qianlan/project/ql_rpc/build/src && /opt/rh/devtoolset-9/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/CMakeFiles/ql_rpc.dir/logger.cpp.o -MF CMakeFiles/ql_rpc.dir/logger.cpp.o.d -o CMakeFiles/ql_rpc.dir/logger.cpp.o -c /root/qianlan/project/ql_rpc/src/logger.cpp

src/CMakeFiles/ql_rpc.dir/logger.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/ql_rpc.dir/logger.cpp.i"
	cd /root/qianlan/project/ql_rpc/build/src && /opt/rh/devtoolset-9/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/qianlan/project/ql_rpc/src/logger.cpp > CMakeFiles/ql_rpc.dir/logger.cpp.i

src/CMakeFiles/ql_rpc.dir/logger.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/ql_rpc.dir/logger.cpp.s"
	cd /root/qianlan/project/ql_rpc/build/src && /opt/rh/devtoolset-9/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/qianlan/project/ql_rpc/src/logger.cpp -o CMakeFiles/ql_rpc.dir/logger.cpp.s

src/CMakeFiles/ql_rpc.dir/ql_rpcapplication.cpp.o: src/CMakeFiles/ql_rpc.dir/flags.make
src/CMakeFiles/ql_rpc.dir/ql_rpcapplication.cpp.o: /root/qianlan/project/ql_rpc/src/ql_rpcapplication.cpp
src/CMakeFiles/ql_rpc.dir/ql_rpcapplication.cpp.o: src/CMakeFiles/ql_rpc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/qianlan/project/ql_rpc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/CMakeFiles/ql_rpc.dir/ql_rpcapplication.cpp.o"
	cd /root/qianlan/project/ql_rpc/build/src && /opt/rh/devtoolset-9/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/CMakeFiles/ql_rpc.dir/ql_rpcapplication.cpp.o -MF CMakeFiles/ql_rpc.dir/ql_rpcapplication.cpp.o.d -o CMakeFiles/ql_rpc.dir/ql_rpcapplication.cpp.o -c /root/qianlan/project/ql_rpc/src/ql_rpcapplication.cpp

src/CMakeFiles/ql_rpc.dir/ql_rpcapplication.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/ql_rpc.dir/ql_rpcapplication.cpp.i"
	cd /root/qianlan/project/ql_rpc/build/src && /opt/rh/devtoolset-9/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/qianlan/project/ql_rpc/src/ql_rpcapplication.cpp > CMakeFiles/ql_rpc.dir/ql_rpcapplication.cpp.i

src/CMakeFiles/ql_rpc.dir/ql_rpcapplication.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/ql_rpc.dir/ql_rpcapplication.cpp.s"
	cd /root/qianlan/project/ql_rpc/build/src && /opt/rh/devtoolset-9/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/qianlan/project/ql_rpc/src/ql_rpcapplication.cpp -o CMakeFiles/ql_rpc.dir/ql_rpcapplication.cpp.s

src/CMakeFiles/ql_rpc.dir/ql_rpcchannel.cpp.o: src/CMakeFiles/ql_rpc.dir/flags.make
src/CMakeFiles/ql_rpc.dir/ql_rpcchannel.cpp.o: /root/qianlan/project/ql_rpc/src/ql_rpcchannel.cpp
src/CMakeFiles/ql_rpc.dir/ql_rpcchannel.cpp.o: src/CMakeFiles/ql_rpc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/qianlan/project/ql_rpc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object src/CMakeFiles/ql_rpc.dir/ql_rpcchannel.cpp.o"
	cd /root/qianlan/project/ql_rpc/build/src && /opt/rh/devtoolset-9/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/CMakeFiles/ql_rpc.dir/ql_rpcchannel.cpp.o -MF CMakeFiles/ql_rpc.dir/ql_rpcchannel.cpp.o.d -o CMakeFiles/ql_rpc.dir/ql_rpcchannel.cpp.o -c /root/qianlan/project/ql_rpc/src/ql_rpcchannel.cpp

src/CMakeFiles/ql_rpc.dir/ql_rpcchannel.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/ql_rpc.dir/ql_rpcchannel.cpp.i"
	cd /root/qianlan/project/ql_rpc/build/src && /opt/rh/devtoolset-9/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/qianlan/project/ql_rpc/src/ql_rpcchannel.cpp > CMakeFiles/ql_rpc.dir/ql_rpcchannel.cpp.i

src/CMakeFiles/ql_rpc.dir/ql_rpcchannel.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/ql_rpc.dir/ql_rpcchannel.cpp.s"
	cd /root/qianlan/project/ql_rpc/build/src && /opt/rh/devtoolset-9/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/qianlan/project/ql_rpc/src/ql_rpcchannel.cpp -o CMakeFiles/ql_rpc.dir/ql_rpcchannel.cpp.s

src/CMakeFiles/ql_rpc.dir/ql_rpcconfig.cpp.o: src/CMakeFiles/ql_rpc.dir/flags.make
src/CMakeFiles/ql_rpc.dir/ql_rpcconfig.cpp.o: /root/qianlan/project/ql_rpc/src/ql_rpcconfig.cpp
src/CMakeFiles/ql_rpc.dir/ql_rpcconfig.cpp.o: src/CMakeFiles/ql_rpc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/qianlan/project/ql_rpc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object src/CMakeFiles/ql_rpc.dir/ql_rpcconfig.cpp.o"
	cd /root/qianlan/project/ql_rpc/build/src && /opt/rh/devtoolset-9/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/CMakeFiles/ql_rpc.dir/ql_rpcconfig.cpp.o -MF CMakeFiles/ql_rpc.dir/ql_rpcconfig.cpp.o.d -o CMakeFiles/ql_rpc.dir/ql_rpcconfig.cpp.o -c /root/qianlan/project/ql_rpc/src/ql_rpcconfig.cpp

src/CMakeFiles/ql_rpc.dir/ql_rpcconfig.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/ql_rpc.dir/ql_rpcconfig.cpp.i"
	cd /root/qianlan/project/ql_rpc/build/src && /opt/rh/devtoolset-9/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/qianlan/project/ql_rpc/src/ql_rpcconfig.cpp > CMakeFiles/ql_rpc.dir/ql_rpcconfig.cpp.i

src/CMakeFiles/ql_rpc.dir/ql_rpcconfig.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/ql_rpc.dir/ql_rpcconfig.cpp.s"
	cd /root/qianlan/project/ql_rpc/build/src && /opt/rh/devtoolset-9/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/qianlan/project/ql_rpc/src/ql_rpcconfig.cpp -o CMakeFiles/ql_rpc.dir/ql_rpcconfig.cpp.s

src/CMakeFiles/ql_rpc.dir/ql_rpccontroller.cpp.o: src/CMakeFiles/ql_rpc.dir/flags.make
src/CMakeFiles/ql_rpc.dir/ql_rpccontroller.cpp.o: /root/qianlan/project/ql_rpc/src/ql_rpccontroller.cpp
src/CMakeFiles/ql_rpc.dir/ql_rpccontroller.cpp.o: src/CMakeFiles/ql_rpc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/qianlan/project/ql_rpc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object src/CMakeFiles/ql_rpc.dir/ql_rpccontroller.cpp.o"
	cd /root/qianlan/project/ql_rpc/build/src && /opt/rh/devtoolset-9/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/CMakeFiles/ql_rpc.dir/ql_rpccontroller.cpp.o -MF CMakeFiles/ql_rpc.dir/ql_rpccontroller.cpp.o.d -o CMakeFiles/ql_rpc.dir/ql_rpccontroller.cpp.o -c /root/qianlan/project/ql_rpc/src/ql_rpccontroller.cpp

src/CMakeFiles/ql_rpc.dir/ql_rpccontroller.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/ql_rpc.dir/ql_rpccontroller.cpp.i"
	cd /root/qianlan/project/ql_rpc/build/src && /opt/rh/devtoolset-9/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/qianlan/project/ql_rpc/src/ql_rpccontroller.cpp > CMakeFiles/ql_rpc.dir/ql_rpccontroller.cpp.i

src/CMakeFiles/ql_rpc.dir/ql_rpccontroller.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/ql_rpc.dir/ql_rpccontroller.cpp.s"
	cd /root/qianlan/project/ql_rpc/build/src && /opt/rh/devtoolset-9/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/qianlan/project/ql_rpc/src/ql_rpccontroller.cpp -o CMakeFiles/ql_rpc.dir/ql_rpccontroller.cpp.s

src/CMakeFiles/ql_rpc.dir/rpcheader.pb.cc.o: src/CMakeFiles/ql_rpc.dir/flags.make
src/CMakeFiles/ql_rpc.dir/rpcheader.pb.cc.o: /root/qianlan/project/ql_rpc/src/rpcheader.pb.cc
src/CMakeFiles/ql_rpc.dir/rpcheader.pb.cc.o: src/CMakeFiles/ql_rpc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/qianlan/project/ql_rpc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object src/CMakeFiles/ql_rpc.dir/rpcheader.pb.cc.o"
	cd /root/qianlan/project/ql_rpc/build/src && /opt/rh/devtoolset-9/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/CMakeFiles/ql_rpc.dir/rpcheader.pb.cc.o -MF CMakeFiles/ql_rpc.dir/rpcheader.pb.cc.o.d -o CMakeFiles/ql_rpc.dir/rpcheader.pb.cc.o -c /root/qianlan/project/ql_rpc/src/rpcheader.pb.cc

src/CMakeFiles/ql_rpc.dir/rpcheader.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/ql_rpc.dir/rpcheader.pb.cc.i"
	cd /root/qianlan/project/ql_rpc/build/src && /opt/rh/devtoolset-9/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/qianlan/project/ql_rpc/src/rpcheader.pb.cc > CMakeFiles/ql_rpc.dir/rpcheader.pb.cc.i

src/CMakeFiles/ql_rpc.dir/rpcheader.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/ql_rpc.dir/rpcheader.pb.cc.s"
	cd /root/qianlan/project/ql_rpc/build/src && /opt/rh/devtoolset-9/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/qianlan/project/ql_rpc/src/rpcheader.pb.cc -o CMakeFiles/ql_rpc.dir/rpcheader.pb.cc.s

src/CMakeFiles/ql_rpc.dir/rpcprovider.cpp.o: src/CMakeFiles/ql_rpc.dir/flags.make
src/CMakeFiles/ql_rpc.dir/rpcprovider.cpp.o: /root/qianlan/project/ql_rpc/src/rpcprovider.cpp
src/CMakeFiles/ql_rpc.dir/rpcprovider.cpp.o: src/CMakeFiles/ql_rpc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/qianlan/project/ql_rpc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object src/CMakeFiles/ql_rpc.dir/rpcprovider.cpp.o"
	cd /root/qianlan/project/ql_rpc/build/src && /opt/rh/devtoolset-9/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/CMakeFiles/ql_rpc.dir/rpcprovider.cpp.o -MF CMakeFiles/ql_rpc.dir/rpcprovider.cpp.o.d -o CMakeFiles/ql_rpc.dir/rpcprovider.cpp.o -c /root/qianlan/project/ql_rpc/src/rpcprovider.cpp

src/CMakeFiles/ql_rpc.dir/rpcprovider.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/ql_rpc.dir/rpcprovider.cpp.i"
	cd /root/qianlan/project/ql_rpc/build/src && /opt/rh/devtoolset-9/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/qianlan/project/ql_rpc/src/rpcprovider.cpp > CMakeFiles/ql_rpc.dir/rpcprovider.cpp.i

src/CMakeFiles/ql_rpc.dir/rpcprovider.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/ql_rpc.dir/rpcprovider.cpp.s"
	cd /root/qianlan/project/ql_rpc/build/src && /opt/rh/devtoolset-9/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/qianlan/project/ql_rpc/src/rpcprovider.cpp -o CMakeFiles/ql_rpc.dir/rpcprovider.cpp.s

src/CMakeFiles/ql_rpc.dir/zookeeperutil.cpp.o: src/CMakeFiles/ql_rpc.dir/flags.make
src/CMakeFiles/ql_rpc.dir/zookeeperutil.cpp.o: /root/qianlan/project/ql_rpc/src/zookeeperutil.cpp
src/CMakeFiles/ql_rpc.dir/zookeeperutil.cpp.o: src/CMakeFiles/ql_rpc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/qianlan/project/ql_rpc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object src/CMakeFiles/ql_rpc.dir/zookeeperutil.cpp.o"
	cd /root/qianlan/project/ql_rpc/build/src && /opt/rh/devtoolset-9/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/CMakeFiles/ql_rpc.dir/zookeeperutil.cpp.o -MF CMakeFiles/ql_rpc.dir/zookeeperutil.cpp.o.d -o CMakeFiles/ql_rpc.dir/zookeeperutil.cpp.o -c /root/qianlan/project/ql_rpc/src/zookeeperutil.cpp

src/CMakeFiles/ql_rpc.dir/zookeeperutil.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/ql_rpc.dir/zookeeperutil.cpp.i"
	cd /root/qianlan/project/ql_rpc/build/src && /opt/rh/devtoolset-9/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/qianlan/project/ql_rpc/src/zookeeperutil.cpp > CMakeFiles/ql_rpc.dir/zookeeperutil.cpp.i

src/CMakeFiles/ql_rpc.dir/zookeeperutil.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/ql_rpc.dir/zookeeperutil.cpp.s"
	cd /root/qianlan/project/ql_rpc/build/src && /opt/rh/devtoolset-9/root/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/qianlan/project/ql_rpc/src/zookeeperutil.cpp -o CMakeFiles/ql_rpc.dir/zookeeperutil.cpp.s

# Object files for target ql_rpc
ql_rpc_OBJECTS = \
"CMakeFiles/ql_rpc.dir/logger.cpp.o" \
"CMakeFiles/ql_rpc.dir/ql_rpcapplication.cpp.o" \
"CMakeFiles/ql_rpc.dir/ql_rpcchannel.cpp.o" \
"CMakeFiles/ql_rpc.dir/ql_rpcconfig.cpp.o" \
"CMakeFiles/ql_rpc.dir/ql_rpccontroller.cpp.o" \
"CMakeFiles/ql_rpc.dir/rpcheader.pb.cc.o" \
"CMakeFiles/ql_rpc.dir/rpcprovider.cpp.o" \
"CMakeFiles/ql_rpc.dir/zookeeperutil.cpp.o"

# External object files for target ql_rpc
ql_rpc_EXTERNAL_OBJECTS =

/root/qianlan/project/ql_rpc/lib/libql_rpc.a: src/CMakeFiles/ql_rpc.dir/logger.cpp.o
/root/qianlan/project/ql_rpc/lib/libql_rpc.a: src/CMakeFiles/ql_rpc.dir/ql_rpcapplication.cpp.o
/root/qianlan/project/ql_rpc/lib/libql_rpc.a: src/CMakeFiles/ql_rpc.dir/ql_rpcchannel.cpp.o
/root/qianlan/project/ql_rpc/lib/libql_rpc.a: src/CMakeFiles/ql_rpc.dir/ql_rpcconfig.cpp.o
/root/qianlan/project/ql_rpc/lib/libql_rpc.a: src/CMakeFiles/ql_rpc.dir/ql_rpccontroller.cpp.o
/root/qianlan/project/ql_rpc/lib/libql_rpc.a: src/CMakeFiles/ql_rpc.dir/rpcheader.pb.cc.o
/root/qianlan/project/ql_rpc/lib/libql_rpc.a: src/CMakeFiles/ql_rpc.dir/rpcprovider.cpp.o
/root/qianlan/project/ql_rpc/lib/libql_rpc.a: src/CMakeFiles/ql_rpc.dir/zookeeperutil.cpp.o
/root/qianlan/project/ql_rpc/lib/libql_rpc.a: src/CMakeFiles/ql_rpc.dir/build.make
/root/qianlan/project/ql_rpc/lib/libql_rpc.a: src/CMakeFiles/ql_rpc.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/root/qianlan/project/ql_rpc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Linking CXX static library /root/qianlan/project/ql_rpc/lib/libql_rpc.a"
	cd /root/qianlan/project/ql_rpc/build/src && $(CMAKE_COMMAND) -P CMakeFiles/ql_rpc.dir/cmake_clean_target.cmake
	cd /root/qianlan/project/ql_rpc/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ql_rpc.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/ql_rpc.dir/build: /root/qianlan/project/ql_rpc/lib/libql_rpc.a
.PHONY : src/CMakeFiles/ql_rpc.dir/build

src/CMakeFiles/ql_rpc.dir/clean:
	cd /root/qianlan/project/ql_rpc/build/src && $(CMAKE_COMMAND) -P CMakeFiles/ql_rpc.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/ql_rpc.dir/clean

src/CMakeFiles/ql_rpc.dir/depend:
	cd /root/qianlan/project/ql_rpc/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/qianlan/project/ql_rpc /root/qianlan/project/ql_rpc/src /root/qianlan/project/ql_rpc/build /root/qianlan/project/ql_rpc/build/src /root/qianlan/project/ql_rpc/build/src/CMakeFiles/ql_rpc.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : src/CMakeFiles/ql_rpc.dir/depend

