add_subdirectory(amlogicengine)
file(GLOB_RECURSE DEVICE_ENGINE_SRC_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "device_register.cc"
        "abstractengine.cc"
    )

message("-----------------${DEVICE_ENGINE_SRC_LIST}===============")
add_library(device_utile SHARED ${DEVICE_ENGINE_SRC_LIST})
target_link_libraries(device_utile amlogic_engine)