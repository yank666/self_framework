include (ExternalProject)
set(EXTERNAL_LIB_CMAKE_CXX_COMPILER "${CMAKE_CXX_COMPILER} -Werror")

if (${ENABLE_ENGINE_TYPE} MATCHES "NB")
    set(EXTRA_CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=$ENV{ANDROID_NDK}/build/cmake/android.toolchain.cmake -DANDROID_NATIVE_API_LEVEL=28
            -DANDROID_NDK=$ENV{ANDROID_NDK} -DANDROID_ABI=armeabi-v7a -DANDROID_TOOLCHAIN_NAME=aarch64-linux-android-clang
            -DANDROID_STL=c++_shared)
else()
    set(EXTRA_CMAKE_ARGS "")
endif()

include_directories(${PROJECT_SOURCE_DIR}/thirdparty/eigen)

include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk/include)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk/include/utils)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk/include/client)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk/include/ops)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk/ovx_inc)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk/ovx_inc/CL)


set(third_party_deps "")
include(${PROJECT_SOURCE_DIR}/cmake_dependency/External_Glog.cmake)
include(${PROJECT_SOURCE_DIR}/cmake_dependency/External_Gtest.cmake)
include(${PROJECT_SOURCE_DIR}/cmake_dependency/External_Protobuf.cmake)
include(${PROJECT_SOURCE_DIR}/cmake_dependency/External_Opencv.cmake)
add_custom_target(third_party ALL DEPENDS ${third_party_deps})


