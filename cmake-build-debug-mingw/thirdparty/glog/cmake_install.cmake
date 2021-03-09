# Install script for directory: D:/workBase/Goodlib/software_constauct_base/thirdparty/glog

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Self_Architecture")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/workBase/Goodlib/software_constauct_base/cmake-build-debug-mingw/thirdparty/glog/libglogd.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/glog" TYPE FILE FILES
    "D:/workBase/Goodlib/software_constauct_base/cmake-build-debug-mingw/thirdparty/glog/glog/logging.h"
    "D:/workBase/Goodlib/software_constauct_base/cmake-build-debug-mingw/thirdparty/glog/glog/raw_logging.h"
    "D:/workBase/Goodlib/software_constauct_base/cmake-build-debug-mingw/thirdparty/glog/glog/stl_logging.h"
    "D:/workBase/Goodlib/software_constauct_base/cmake-build-debug-mingw/thirdparty/glog/glog/vlog_is_on.h"
    "D:/workBase/Goodlib/software_constauct_base/thirdparty/glog/src/glog/log_severity.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "D:/workBase/Goodlib/software_constauct_base/cmake-build-debug-mingw/thirdparty/glog/libglog.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  
set (glog_FULL_CMake_DATADIR "\${CMAKE_CURRENT_LIST_DIR}/../../../share/glog/cmake")
configure_file ("D:/workBase/Goodlib/software_constauct_base/thirdparty/glog/glog-modules.cmake.in"
  "D:/workBase/Goodlib/software_constauct_base/cmake-build-debug-mingw/thirdparty/glog/CMakeFiles/CMakeTmp/glog-modules.cmake" @ONLY)
file (INSTALL
  "D:/workBase/Goodlib/software_constauct_base/cmake-build-debug-mingw/thirdparty/glog/CMakeFiles/CMakeTmp/glog-modules.cmake"
  DESTINATION
  "${CMAKE_INSTALL_PREFIX}/lib/cmake/glog")

endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/glog" TYPE FILE FILES
    "D:/workBase/Goodlib/software_constauct_base/cmake-build-debug-mingw/thirdparty/glog/glog-config.cmake"
    "D:/workBase/Goodlib/software_constauct_base/cmake-build-debug-mingw/thirdparty/glog/glog-config-version.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/glog" TYPE DIRECTORY FILES "D:/workBase/Goodlib/software_constauct_base/cmake-build-debug-mingw/thirdparty/glog/share/glog/cmake" FILES_MATCHING REGEX "/[^/]*\\.cmake$")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/glog/glog-targets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/glog/glog-targets.cmake"
         "D:/workBase/Goodlib/software_constauct_base/cmake-build-debug-mingw/thirdparty/glog/CMakeFiles/Export/lib/cmake/glog/glog-targets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/glog/glog-targets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/glog/glog-targets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/glog" TYPE FILE FILES "D:/workBase/Goodlib/software_constauct_base/cmake-build-debug-mingw/thirdparty/glog/CMakeFiles/Export/lib/cmake/glog/glog-targets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/glog" TYPE FILE FILES "D:/workBase/Goodlib/software_constauct_base/cmake-build-debug-mingw/thirdparty/glog/CMakeFiles/Export/lib/cmake/glog/glog-targets-debug.cmake")
  endif()
endif()

