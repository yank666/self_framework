file(GLOB_RECURSE DEVICE_ENGINE_SRC_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "device_register.cc"
        "abstractengine.cc"
)
add_library(device_utile SHARED ${DEVICE_ENGINE_SRC_LIST})


if(${ENABLE_ENGINE_TYPE} MATCHES "NB")
    add_subdirectory(amlogicengine)
    target_link_libraries(device_utile amlogic_engine)
elseif(${ENABLE_ENGINE_TYPE} MATCHES "TRT")
    add_subdirectory(trtengine)
    target_link_libraries(device_utile trt_engine)
endif()

