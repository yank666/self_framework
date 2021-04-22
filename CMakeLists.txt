cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(Self_Architecture)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fpic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic")

# compile thirdparty
set(protobuf_BUILD_TESTS OFF CACHE BOOL "protobuf test compile option" FORCE)
set(protobuf_BUILD_SHARED_LIBS OFF CACHE BOOL "protobuf test compile option" FORCE)
set(PROTOBUF_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/protobuf)
include_directories(${PROTOBUF_DIR}/src)
add_subdirectory(${PROTOBUF_DIR}/cmake)


set(BUILD_TESTING OFF CACHE BOOL "gtest test compile option" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "glog test compile option" FORCE)
set(BUILD_SHARED_LIBS ON CACHE BOOL "glog test compile option" FORCE)
set(GTEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/gtest/)
include_directories(${GTEST_DIR}/googletest/include)
add_subdirectory(${GTEST_DIR})

set(BUILD_TESTING OFF CACHE BOOL "glog test compile option" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "glog test compile option" FORCE)
set(BUILD_SHARED_LIBS ON CACHE BOOL "glog test compile option" FORCE)
set(GLOG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glog)
include_directories(${GLOG_DIR}/src)
add_subdirectory(${GLOG_DIR})

include(${PROJECT_SOURCE_DIR}/cmake_dependency/SuperBuild.cmake)

include_directories(${PROJECT_SOURCE_DIR}/thirdparty/eigen)
MESSAGE("++++++++++++++++++++++++${Eigen_SOURCE_DIR}+++++++++++++++")


#include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk/include)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk/include/utils)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk/include/client)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk/include/ops)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk/ovx_inc)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk/ovx_inc/CL)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE  proto_files ${CMAKE_CURRENT_SOURCE_DIR}/modules/*.proto)
foreach(fil ${proto_files})
    get_filename_component(abs_fil ${fil} ABSOLUTE)
    get_filename_component(fil_we ${fil} NAME_WE)
    get_filename_component(file_dir ${fil} PATH)

    list(APPEND srcs_var "${CMAKE_CURRENT_SOURCE_DIR}/modules/proto/${fil_we}.pb.cc")
    list(APPEND hdrs_var "${CMAKE_CURRENT_SOURCE_DIR}/modules/proto/${fil_we}.pb.h")
endforeach()

set_source_files_properties(${srcs_var} ${hdrs_var} PROPERTIES GENERATED TRUE)
add_custom_target(generate_proto ALL
        DEPENDS ${srcs_var} ${hdrs_var}
        COMMENT "generate message target"
        VERBATIM
        )

add_subdirectory(src/pipeline)
add_subdirectory(src/parse)

if (BUILD_TESTCASE)
    add_subdirectory(tests)
endif()
include(${PROJECT_SOURCE_DIR}/cmake_dependency/package.cmake)

