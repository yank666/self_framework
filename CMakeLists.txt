cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(Self_Architecture)
set(CMAKE_CXX_STANDARD 14)
#option
option(ENABLE_COMPILE_PROTO "whether add compile protobuf" ON)

#find package
#find_package(OpenCV REQUIRED)
#find_package(OpenMP REQUIRED)
#find_package(PythonInterp 3.8 REQUIRED) #${PYTHON_EXECUTABLE}  python.exe 强制找到Python >= 3.8.0


# compile thirdparty
set(GTEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/gtest/)
#set(PROTOBUF_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/protobuf)
set(GLOG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glog)
#set(EIGEN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/eigen)

#include(FetchContent)
include_directories(${GTEST_DIR}/googletest/include)
include_directories(${PROTOBUF_DIR}/src)
include_directories(${GLOG_DIR}/src)
include_directories(${OpenCV_INCLUDE_DIRS})
#include_directories(${EIGEN_DIR})

add_subdirectory(${GTEST_DIR})
#add_subdirectory(${PROTOBUF_DIR}/cmake)
add_subdirectory(${GLOG_DIR})
#add_subdirectory(${EIGEN_DIR})

#file(GLOB_RECURSE  proto_files ${CMAKE_CURRENT_SOURCE_DIR}/modules/*.proto)
#foreach(fil ${proto_files})
#    get_filename_component(abs_fil ${fil} ABSOLUTE)
#    get_filename_component(fil_we ${fil} NAME_WE)
#    get_filename_component(file_dir ${fil} PATH)
#
#    list(APPEND srcs_var "${CMAKE_CURRENT_SOURCE_DIR}/modules/${fil_we}.pb.cc")
#    list(APPEND hdrs_var "${CMAKE_CURRENT_SOURCE_DIR}/modules/${fil_we}.pb.h")
#
#    add_custom_command(
#            OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/modules/${fil_we}.pb.cc"
#            "${CMAKE_CURRENT_SOURCE_DIR}/modules/${fil_we}.pb.h"
#            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#            COMMAND protoc -I${file_dir} --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/modules ${abs_fil}
#            DEPENDS ${abs_file} protoc
#            COMMENT "Running C++ protocol buffer compiler on ${fil}" VERBATIM )
#
#endforeach()

add_executable(main main.cpp)
target_link_libraries(main glog::glog)

#target_link_libraries(test_base64encode glog::glog ${OpenCV_LIBS} OpenMP::OpenMP_CX
add_subdirectory(src/pipeline)
target_link_libraries(main pipeline)


