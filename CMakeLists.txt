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

set(protobuf_BUILD_TESTS OFF CACHE BOOL "protobuf test compile option" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "glog test compile option" FORCE)


include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${GTEST_DIR}/googletest/include)
include_directories(${PROTOBUF_DIR}/src)
include_directories(${GLOG_DIR}/src)
include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk/include)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk/include/utils)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk/include/client)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk/include/ops)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk/ovx_inc)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk/ovx_inc/CL)


add_subdirectory(${GTEST_DIR})
add_subdirectory(${PROTOBUF_DIR}/cmake)
add_subdirectory(${GLOG_DIR})

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

add_executable(main main.cpp)
target_link_libraries(main glog::glog)

#target_link_libraries(test_base64encode glog::glog ${OpenCV_LIBS} OpenMP::OpenMP_CXX)
add_subdirectory(src/pipeline)
add_subdirectory(src/parse)
add_subdirectory(src/deviceengine)

target_link_libraries(main parseconfig)
target_link_libraries(pipeline device_utile)
target_link_libraries(main pipeline)


