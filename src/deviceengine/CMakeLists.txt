file(GLOB_RECURSE DEVICE_ENGINE_SRC_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "*.cc"
        )
add_subdirectory(amlogicengine)
add_library(device_utile SHARED ${DEVICE_ENGINE_SRC_LIST})