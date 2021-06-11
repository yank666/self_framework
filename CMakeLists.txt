cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(Self_Architecture)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fpic -pthread -Wno-deprecated-declarations -g -ggdb")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic -pthread -Wno-deprecated-declarations  -g -ggdb")

include(${PROJECT_SOURCE_DIR}/cmake_dependency/External_SuperBuild.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty)


file(GLOB_RECURSE  proto_files ${CMAKE_CURRENT_SOURCE_DIR}/modules/*.proto)
foreach(fil ${proto_files})
    get_filename_component(abs_fil ${fil} ABSOLUTE)
    get_filename_component(fil_we ${fil} NAME_WE)
    get_filename_component(file_dir ${fil} PATH)

    list(APPEND srcs_var "${CMAKE_CURRENT_SOURCE_DIR}/modules/proto/${fil_we}.pb.cc")
    list(APPEND hdrs_var "${CMAKE_CURRENT_SOURCE_DIR}/modules/proto/${fil_we}.pb.h")
endforeach()

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")

add_subdirectory(thirdparty/tracker)
add_subdirectory(src/pipeline)
add_subdirectory(src/parse)

if (BUILD_TESTCASE)
    add_subdirectory(tests)
endif()
include(${PROJECT_SOURCE_DIR}/cmake_dependency/Package.cmake)


#add_executable(main main.cc)
#add_dependencies(main External_opencv External_ffmpeg)
#target_link_libraries(main ${FFMPEG_LIBS} ${OPENCV_LIBS})

