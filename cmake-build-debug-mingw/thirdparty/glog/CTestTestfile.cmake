# CMake generated Testfile for 
# Source directory: D:/workBase/Goodlib/software_constauct_base/thirdparty/glog
# Build directory: D:/workBase/Goodlib/software_constauct_base/cmake-build-debug-mingw/thirdparty/glog
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(demangle "D:/workBase/Goodlib/software_constauct_base/cmake-build-debug-mingw/thirdparty/glog/demangle_unittest.exe")
set_tests_properties(demangle PROPERTIES  _BACKTRACE_TRIPLES "D:/workBase/Goodlib/software_constauct_base/thirdparty/glog/CMakeLists.txt;704;add_test;D:/workBase/Goodlib/software_constauct_base/thirdparty/glog/CMakeLists.txt;0;")
add_test(logging "D:/workBase/Goodlib/software_constauct_base/cmake-build-debug-mingw/thirdparty/glog/logging_unittest.exe")
set_tests_properties(logging PROPERTIES  _BACKTRACE_TRIPLES "D:/workBase/Goodlib/software_constauct_base/thirdparty/glog/CMakeLists.txt;705;add_test;D:/workBase/Goodlib/software_constauct_base/thirdparty/glog/CMakeLists.txt;0;")
add_test(stl_logging "D:/workBase/Goodlib/software_constauct_base/cmake-build-debug-mingw/thirdparty/glog/stl_logging_unittest.exe")
set_tests_properties(stl_logging PROPERTIES  _BACKTRACE_TRIPLES "D:/workBase/Goodlib/software_constauct_base/thirdparty/glog/CMakeLists.txt;716;add_test;D:/workBase/Goodlib/software_constauct_base/thirdparty/glog/CMakeLists.txt;0;")
add_test(cmake_package_config_init "D:/softwarePath/toolSoftware/Clion/CLion 2019.3.6/bin/cmake/win/bin/cmake.exe" "-DTEST_BINARY_DIR=D:/workBase/Goodlib/software_constauct_base/cmake-build-debug-mingw/thirdparty/glog/test_package_config" "-DINITIAL_CACHE=D:/workBase/Goodlib/software_constauct_base/cmake-build-debug-mingw/thirdparty/glog/test_package_config/glog_package_config_initial_cache.cmake" "-DCACHEVARS=set (BUILD_GMOCK ON CACHE BOOL \"Builds the googlemock subproject\")
set (BUILD_SHARED_LIBS OFF CACHE BOOL \"Build shared libraries\")
set (BUILD_TESTING ON CACHE BOOL \"Build the testing tree.\")
set (BZRCOMMAND \"BZRCOMMAND-NOTFOUND\" CACHE FILEPATH \"Path to a program.\")
set (CMAKE_AR \"C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0/bin/ar.exe\" CACHE FILEPATH \"Path to a program.\")
set (CMAKE_AR \"C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0/bin/ar.exe\" CACHE FILEPATH \"Path to a program.\")
set (CMAKE_BUILD_TYPE \"debug\" CACHE STRING \"Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel ...\")
set (CMAKE_CODEBLOCKS_EXECUTABLE \"CMAKE_CODEBLOCKS_EXECUTABLE-NOTFOUND\" CACHE FILEPATH \"The CodeBlocks executable\")
set (CMAKE_COLOR_MAKEFILE ON CACHE BOOL \"Enable/Disable color output during build.\")
set (CMAKE_CXX_COMPILER \"C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0/bin/g++.exe\" CACHE FILEPATH \"CXX compiler\")
set (CMAKE_CXX_COMPILER \"C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0/bin/g++.exe\" CACHE FILEPATH \"CXX compiler\")
set (CMAKE_CXX_COMPILER_AR \"C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0/bin/gcc-ar.exe\" CACHE FILEPATH \"A wrapper around 'ar' adding the appropriate '--plugin' option for the GCC compiler\")
set (CMAKE_CXX_COMPILER_AR \"C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0/bin/gcc-ar.exe\" CACHE FILEPATH \"A wrapper around 'ar' adding the appropriate '--plugin' option for the GCC compiler\")
set (CMAKE_CXX_COMPILER_RANLIB \"C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0/bin/gcc-ranlib.exe\" CACHE FILEPATH \"A wrapper around 'ranlib' adding the appropriate '--plugin' option for the GCC compiler\")
set (CMAKE_CXX_COMPILER_RANLIB \"C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0/bin/gcc-ranlib.exe\" CACHE FILEPATH \"A wrapper around 'ranlib' adding the appropriate '--plugin' option for the GCC compiler\")
set (CMAKE_CXX_FLAGS_DEBUG \"-g\" CACHE STRING \"Flags used by the CXX compiler during DEBUG builds.\")
set (CMAKE_CXX_FLAGS_MINSIZEREL \"-Os -DNDEBUG\" CACHE STRING \"Flags used by the CXX compiler during MINSIZEREL builds.\")
set (CMAKE_CXX_FLAGS_RELEASE \"-O3 -DNDEBUG\" CACHE STRING \"Flags used by the CXX compiler during RELEASE builds.\")
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO \"-O2 -g -DNDEBUG\" CACHE STRING \"Flags used by the CXX compiler during RELWITHDEBINFO builds.\")
set (CMAKE_CXX_STANDARD_LIBRARIES \"-lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32\" CACHE STRING \"Libraries linked by default with all C++ applications.\")
set (CMAKE_C_COMPILER \"C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0/bin/gcc.exe\" CACHE FILEPATH \"C compiler\")
set (CMAKE_C_COMPILER \"C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0/bin/gcc.exe\" CACHE FILEPATH \"C compiler\")
set (CMAKE_C_COMPILER_AR \"C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0/bin/gcc-ar.exe\" CACHE FILEPATH \"A wrapper around 'ar' adding the appropriate '--plugin' option for the GCC compiler\")
set (CMAKE_C_COMPILER_AR \"C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0/bin/gcc-ar.exe\" CACHE FILEPATH \"A wrapper around 'ar' adding the appropriate '--plugin' option for the GCC compiler\")
set (CMAKE_C_COMPILER_RANLIB \"C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0/bin/gcc-ranlib.exe\" CACHE FILEPATH \"A wrapper around 'ranlib' adding the appropriate '--plugin' option for the GCC compiler\")
set (CMAKE_C_COMPILER_RANLIB \"C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0/bin/gcc-ranlib.exe\" CACHE FILEPATH \"A wrapper around 'ranlib' adding the appropriate '--plugin' option for the GCC compiler\")
set (CMAKE_C_FLAGS_DEBUG \"-g\" CACHE STRING \"Flags used by the C compiler during DEBUG builds.\")
set (CMAKE_C_FLAGS_MINSIZEREL \"-Os -DNDEBUG\" CACHE STRING \"Flags used by the C compiler during MINSIZEREL builds.\")
set (CMAKE_C_FLAGS_RELEASE \"-O3 -DNDEBUG\" CACHE STRING \"Flags used by the C compiler during RELEASE builds.\")
set (CMAKE_C_FLAGS_RELWITHDEBINFO \"-O2 -g -DNDEBUG\" CACHE STRING \"Flags used by the C compiler during RELWITHDEBINFO builds.\")
set (CMAKE_C_STANDARD_LIBRARIES \"-lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32\" CACHE STRING \"Libraries linked by default with all C applications.\")
set (CMAKE_GNUtoMS OFF CACHE BOOL \"Convert GNU import libraries to MS format (requires Visual Studio)\")
set (CMAKE_INSTALL_BINDIR \"bin\" CACHE PATH \"User executables (bin)\")
set (CMAKE_INSTALL_DATAROOTDIR \"share\" CACHE PATH \"Read-only architecture-independent data root (share)\")
set (CMAKE_INSTALL_INCLUDEDIR \"include\" CACHE PATH \"C header files (include)\")
set (CMAKE_INSTALL_LIBDIR \"lib\" CACHE PATH \"Object code libraries (lib)\")
set (CMAKE_INSTALL_LIBEXECDIR \"libexec\" CACHE PATH \"Program executables (libexec)\")
set (CMAKE_INSTALL_LOCALSTATEDIR \"var\" CACHE PATH \"Modifiable single-machine data (var)\")
set (CMAKE_INSTALL_OLDINCLUDEDIR \"/usr/include\" CACHE PATH \"C header files for non-gcc (/usr/include)\")
set (CMAKE_INSTALL_PREFIX \"C:/Program Files (x86)/Self_Architecture\" CACHE PATH \"Install path prefix, prepended onto install directories.\")
set (CMAKE_INSTALL_SBINDIR \"sbin\" CACHE PATH \"System admin executables (sbin)\")
set (CMAKE_INSTALL_SHAREDSTATEDIR \"com\" CACHE PATH \"Modifiable architecture-independent data (com)\")
set (CMAKE_INSTALL_SYSCONFDIR \"etc\" CACHE PATH \"Read-only single-machine data (etc)\")
set (CMAKE_LINKER \"C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0/bin/ld.exe\" CACHE FILEPATH \"Path to a program.\")
set (CMAKE_LINKER \"C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0/bin/ld.exe\" CACHE FILEPATH \"Path to a program.\")
set (CMAKE_NM \"C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0/bin/nm.exe\" CACHE FILEPATH \"Path to a program.\")
set (CMAKE_OBJCOPY \"C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0/bin/objcopy.exe\" CACHE FILEPATH \"Path to a program.\")
set (CMAKE_OBJDUMP \"C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0/bin/objdump.exe\" CACHE FILEPATH \"Path to a program.\")
set (CMAKE_RANLIB \"C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0/bin/ranlib.exe\" CACHE FILEPATH \"Path to a program.\")
set (CMAKE_RANLIB \"C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0/bin/ranlib.exe\" CACHE FILEPATH \"Path to a program.\")
set (CMAKE_RC_COMPILER \"C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0/bin/windres.exe\" CACHE FILEPATH \"RC compiler\")
set (CMAKE_RC_COMPILER \"C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0/bin/windres.exe\" CACHE FILEPATH \"RC compiler\")
set (CMAKE_SH \"CMAKE_SH-NOTFOUND\" CACHE FILEPATH \"Path to a program.\")
set (CMAKE_SKIP_INSTALL_RPATH NO CACHE BOOL \"If set, runtime paths are not added when installing shared libraries, but are added when building.\")
set (CMAKE_SKIP_RPATH NO CACHE BOOL \"If set, runtime paths are not added when using shared libraries.\")
set (CMAKE_STRIP \"C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0/bin/strip.exe\" CACHE FILEPATH \"Path to a program.\")
set (CMAKE_VERBOSE_MAKEFILE FALSE CACHE BOOL \"If this value is on, makefiles will be generated without the .SILENT directive, and all commands will be echoed to the console during the make.  This is useful for debugging only. With Visual Studio IDE projects all commands are done without /nologo.\")
set (COVERAGE_COMMAND \"C:/Program Files/mingw-w64/x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0/bin/gcov.exe\" CACHE FILEPATH \"Path to the coverage program that CTest uses for performing coverage inspection\")
set (COVERAGE_EXTRA_FLAGS \"-l\" CACHE STRING \"Extra command line flags to pass to the coverage tool\")
set (CPACK_BINARY_7Z OFF CACHE BOOL \"Enable to build 7-Zip packages\")
set (CPACK_BINARY_IFW OFF CACHE BOOL \"Enable to build IFW packages\")
set (CPACK_BINARY_NSIS ON CACHE BOOL \"Enable to build NSIS packages\")
set (CPACK_BINARY_NUGET OFF CACHE BOOL \"Enable to build NuGet packages\")
set (CPACK_BINARY_WIX OFF CACHE BOOL \"Enable to build WiX packages\")
set (CPACK_BINARY_ZIP OFF CACHE BOOL \"Enable to build ZIP packages\")
set (CPACK_SOURCE_7Z ON CACHE BOOL \"Enable to build 7-Zip source packages\")
set (CPACK_SOURCE_ZIP ON CACHE BOOL \"Enable to build ZIP source packages\")
set (CTEST_SUBMIT_RETRY_COUNT \"3\" CACHE STRING \"How many times to retry timed-out CTest submissions.\")
set (CTEST_SUBMIT_RETRY_DELAY \"5\" CACHE STRING \"How long to wait between timed-out CTest submissions.\")
set (CVSCOMMAND \"CVSCOMMAND-NOTFOUND\" CACHE FILEPATH \"Path to a program.\")
set (CVS_UPDATE_OPTIONS \"-d -A -P\" CACHE STRING \"Options passed to the cvs update command.\")
set (DART_TESTING_TIMEOUT \"1500\" CACHE STRING \"Maximum time allowed before CTest will kill the test.\")
set (ENABLE_COMPILE_PROTO ON CACHE BOOL \"whether add compile protobuf\")
set (ENABLE_ST ON CACHE BOOL \"default include test st\")
set (GITCOMMAND \"D:/softwarePath/toolSoftware/Git/cmd/git.exe\" CACHE FILEPATH \"Path to a program.\")
set (HGCOMMAND \"HGCOMMAND-NOTFOUND\" CACHE FILEPATH \"Path to a program.\")
set (INSTALL_GTEST ON CACHE BOOL \"Enable installation of googletest. (Projects embedding googletest may want to turn this OFF.)\")
set (MAKECOMMAND \"D:/softwarePath/toolSoftware/Clion/CLion\\ 2019.3.6/bin/cmake/win/bin/cmake.exe --build . --config \\\"\${CTEST_CONFIGURATION_TYPE}\\\" -- -i\" CACHE STRING \"Command to build the project\")
set (MEMORYCHECK_COMMAND \"MEMORYCHECK_COMMAND-NOTFOUND\" CACHE FILEPATH \"Path to the memory checking command, used for memory error detection.\")
set (P4COMMAND \"P4COMMAND-NOTFOUND\" CACHE FILEPATH \"Path to a program.\")
set (PRINT_UNSYMBOLIZED_STACK_TRACES OFF CACHE BOOL \"Print file offsets in traces instead of symbolizing\")
set (PYTHON_EXECUTABLE \"D:/softwarePath/toolSoftware/Anacoda/python.exe\" CACHE FILEPATH \"Path to a program.\")
set (SITE \"L-R90YLNSZ-1032\" CACHE STRING \"Name of the computer/site where compile is being run\")
set (SLURM_SBATCH_COMMAND \"SLURM_SBATCH_COMMAND-NOTFOUND\" CACHE FILEPATH \"Path to the SLURM sbatch executable\")
set (SLURM_SRUN_COMMAND \"SLURM_SRUN_COMMAND-NOTFOUND\" CACHE FILEPATH \"Path to the SLURM srun executable\")
set (SVNCOMMAND \"SVNCOMMAND-NOTFOUND\" CACHE FILEPATH \"Path to a program.\")
set (Unwind_INCLUDE_DIR \"Unwind_INCLUDE_DIR-NOTFOUND\" CACHE PATH \"unwind include directory\")
set (Unwind_LIBRARY \"Unwind_LIBRARY-NOTFOUND\" CACHE FILEPATH \"unwind library\")
set (Unwind_PLATFORM_LIBRARY \"Unwind_PLATFORM_LIBRARY-NOTFOUND\" CACHE FILEPATH \"unwind library platform\")
set (WITH_GFLAGS ON CACHE BOOL \"Use gflags\")
set (WITH_PKGCONFIG ON CACHE BOOL \"Enable pkg-config support\")
set (WITH_SYMBOLIZE ON CACHE BOOL \"Enable symbolize module\")
set (WITH_THREADS ON CACHE BOOL \"Enable multithreading support\")
set (WITH_TLS ON CACHE BOOL \"Enable Thread Local Storage (TLS) support\")
set (WITH_UNWIND ON CACHE BOOL \"Enable libunwind support\")
set (gflags_DIR \"gflags_DIR-NOTFOUND\" CACHE PATH \"The directory containing a CMake configuration file for gflags.\")
set (gmock_build_tests OFF CACHE BOOL \"Build all of Google Mock's own tests.\")
set (gtest_build_samples OFF CACHE BOOL \"Build gtest's sample programs.\")
set (gtest_build_tests OFF CACHE BOOL \"Build all of gtest's own tests.\")
set (gtest_disable_pthreads OFF CACHE BOOL \"Disable uses of pthreads in gtest.\")
set (gtest_force_shared_crt OFF CACHE BOOL \"Use shared (DLL) run-time lib even when Google Test is built as static lib.\")
set (gtest_hide_internal_symbols OFF CACHE BOOL \"Build gtest with internal symbols hidden in shared libraries.\")
" "-P" "D:/workBase/Goodlib/software_constauct_base/thirdparty/glog/cmake/TestInitPackageConfig.cmake")
set_tests_properties(cmake_package_config_init PROPERTIES  FIXTURES_SETUP "cmake_package_config" _BACKTRACE_TRIPLES "D:/workBase/Goodlib/software_constauct_base/thirdparty/glog/CMakeLists.txt;731;add_test;D:/workBase/Goodlib/software_constauct_base/thirdparty/glog/CMakeLists.txt;0;")
add_test(cmake_package_config_generate "D:/softwarePath/toolSoftware/Clion/CLion 2019.3.6/bin/cmake/win/bin/cmake.exe" "-DPATH=C:\\Program Files\\mingw-w64\\x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0\\bin" "D:\\softwarePath\\toolSoftware\\Anacoda" "D:\\softwarePath\\toolSoftware\\Anacoda\\Library\\mingw-w64\\bin" "D:\\softwarePath\\toolSoftware\\Anacoda\\Library\\usr\\bin" "D:\\softwarePath\\toolSoftware\\Anacoda\\Library\\bin" "D:\\softwarePath\\toolSoftware\\Anacoda\\Scripts" "C:\\Windows\\system32" "C:\\Windows" "C:\\Windows\\System32\\Wbem" "C:\\Windows\\System32\\WindowsPowerShell\\v1.0;C:\\Windows\\System32\\OpenSSH;D:\\softwarePath\\toolSoftware\\Git\\cmd" "D:\\softwarePath\\toolSoftware\\adb" "C:\\Program Files\\mingw-w64\\x86_64-7.3.0-release-posix-sjlj-rt_v5-rev0\\bin" "D:\\workBase\\Goodlib\\libopencv\\install\\x64\\mingw\\bin" "D:\\softwarePath\\toolSoftware\\VeriSilicon\\VivanteIDE4.3.0\\ide" "C:\\Users\\yankai\\AppData\\Local\\Microsoft\\WindowsApps" "D:\\softwarePath\\toolSoftware\\Clion\\CLion 2019.3.6\\bin" "D:\\softwarePath\\toolSoftware\\Microsoft VS Code\\bin" "D:\\softwarePath\\toolSoftware\\PyCharm 2020.3\\bin" "-DINITIAL_CACHE=D:/workBase/Goodlib/software_constauct_base/cmake-build-debug-mingw/thirdparty/glog/test_package_config/glog_package_config_initial_cache.cmake" "-DTEST_BINARY_DIR=D:/workBase/Goodlib/software_constauct_base/cmake-build-debug-mingw/thirdparty/glog/test_package_config/working_config" "-DSOURCE_DIR=D:/workBase/Goodlib/software_constauct_base/thirdparty/glog/src/package_config_unittest/working_config" "-DPACKAGE_DIR=D:/workBase/Goodlib/software_constauct_base/cmake-build-debug-mingw/thirdparty/glog" "-DGENERATOR=MinGW Makefiles" "-P" "D:/workBase/Goodlib/software_constauct_base/thirdparty/glog/cmake/TestPackageConfig.cmake")
set_tests_properties(cmake_package_config_generate PROPERTIES  FIXTURES_REQUIRED "cmake_package_config" FIXTURES_SETUP "cmake_package_config_working" _BACKTRACE_TRIPLES "D:/workBase/Goodlib/software_constauct_base/thirdparty/glog/CMakeLists.txt;738;add_test;D:/workBase/Goodlib/software_constauct_base/thirdparty/glog/CMakeLists.txt;0;")
add_test(cmake_package_config_build "D:/softwarePath/toolSoftware/Clion/CLion 2019.3.6/bin/cmake/win/bin/cmake.exe" "--build" "D:/workBase/Goodlib/software_constauct_base/cmake-build-debug-mingw/thirdparty/glog/test_package_config/working_config")
set_tests_properties(cmake_package_config_build PROPERTIES  FIXTURES_REQUIRED "cmake_package_config;cmake_package_config_working" _BACKTRACE_TRIPLES "D:/workBase/Goodlib/software_constauct_base/thirdparty/glog/CMakeLists.txt;748;add_test;D:/workBase/Goodlib/software_constauct_base/thirdparty/glog/CMakeLists.txt;0;")
add_test(cmake_package_config_cleanup "D:/softwarePath/toolSoftware/Clion/CLion 2019.3.6/bin/cmake/win/bin/cmake.exe" "-E" "remove_directory" "D:/workBase/Goodlib/software_constauct_base/cmake-build-debug-mingw/thirdparty/glog/test_package_config")
set_tests_properties(cmake_package_config_cleanup PROPERTIES  FIXTURES_CLEANUP "cmake_package_config" _BACKTRACE_TRIPLES "D:/workBase/Goodlib/software_constauct_base/thirdparty/glog/CMakeLists.txt;752;add_test;D:/workBase/Goodlib/software_constauct_base/thirdparty/glog/CMakeLists.txt;0;")