cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(Self_Architecture)
set(CMAKE_CXX_STANDARD 14)
#option
option(ENABLE_COMPILE_PROTO "whether add compile protobuf" ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fpic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic")
#find_package(OpenCV REQUIRED)
#find_package(OpenMP REQUIRED)
#find_package(PythonInterp 3.8 REQUIRED) #${PYTHON_EXECUTABLE}  python.exe 强制找到Python >= 3.8.0


# compile thirdparty
set(GTEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/gtest/)
set(PROTOBUF_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/protobuf)
set(GLOG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glog)
#set(EIGEN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/eigen)

#include(FetchContent)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${GTEST_DIR}/googletest/include)
include_directories(${PROTOBUF_DIR}/src)
include_directories(${GLOG_DIR}/src)
include_directories(${OpenCV_INCLUDE_DIRS})
#include_directories(${EIGEN_DIR})

add_subdirectory(${GTEST_DIR})
add_subdirectory(${PROTOBUF_DIR}/cmake)
add_subdirectory(${GLOG_DIR})
#add_subdirectory(${EIGEN_DIR})


#file(GLOB_RECURSE  proto_files ${CMAKE_CURRENT_SOURCE_DIR}/modules/proto/*.proto)
#set(srcs_var "")
#set(hdrs_var "")
#foreach(fil ${proto_files})
#    get_filename_component(abs_fil ${fil} ABSOLUTE)
#    get_filename_component(fil_we ${fil} NAME_WE)
#    get_filename_component(file_dir ${fil} PATH)
#
#    list(APPEND srcs_var "${CMAKE_CURRENT_SOURCE_DIR}/modules/proto/${fil_we}.pb.cc")
#    list(APPEND hdrs_var "${CMAKE_CURRENT_SOURCE_DIR}/modules/proto/${fil_we}.pb.h")
#
#    add_custom_command(
#            OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/modules/${fil_we}.pb.cc"
#            "${CMAKE_CURRENT_SOURCE_DIR}/modules/${fil_we}.pb.h"
#            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#            COMMAND protoc ARGS -I${file_dir} --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/modules ${abs_fil}
#            DEPENDS ${fil} protoc
#            COMMENT "Running C++ protocol buffer compiler on ${fil}" VERBATIM )
#
#endforeach()
#set_source_files_properties(${srcs_var} ${hdrs_var} PROPERTIES GENERATED TRUE)
#add_custom_target(generate_proto ALL
#        DEPENDS ${srcs_var} ${hdrs_var}
#        COMMENT "generate message target"
#        VERBATIM
#        )
file(GLOB_RECURSE  proto_files ${CMAKE_CURRENT_SOURCE_DIR}/modules/*.proto)
foreach(fil ${proto_files})
    get_filename_component(abs_fil ${fil} ABSOLUTE)
    get_filename_component(fil_we ${fil} NAME_WE)
    get_filename_component(file_dir ${fil} PATH)

    list(APPEND srcs_var "${CMAKE_CURRENT_SOURCE_DIR}/modules/proto/${fil_we}.pb.cc")
    list(APPEND hdrs_var "${CMAKE_CURRENT_SOURCE_DIR}/modules/proto/${fil_we}.pb.h")

    add_custom_command(
            OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/modules/${fil_we}.pb.cc"
            "${CMAKE_CURRENT_SOURCE_DIR}/modules/${fil_we}.pb.h"
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            COMMAND protoc -I${file_dir} --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/modules/proto ${abs_fil}
            DEPENDS ${abs_file} protoc
            COMMENT "Running C++ protocol buffer compiler on ${fil}" VERBATIM )

endforeach()
add_library(proto_OBJS SHARED ${srcs_var})

add_executable(main main.cpp)
target_link_libraries(main glog::glog)

#target_link_libraries(test_base64encode glog::glog ${OpenCV_LIBS} OpenMP::OpenMP_CX
add_subdirectory(src/pipeline)
add_subdirectory(src/parse)
add_subdirectory(src/deviceengine)
message("==================${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/thirdparty/protobuf/cmake========================")
target_link_directories(parseconfig PUBLIC "/home/yankai.yan/workbase/codeLib/refactor/cmake-build-debug/thirdparty/protobuf/cmake")
target_link_libraries(parseconfig proto_OBJS protobufd)
target_link_libraries(main pipeline parseconfig)


