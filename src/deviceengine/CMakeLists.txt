file(GLOB DEVICE_ENGINE_SRC_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "*.cc"
)

if(${ENABLE_ENGINE_TYPE} MATCHES "NB")
    file(GLOB AMLOGICAL_SRC_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "amlogicengine/*.cc")
    list(APPEND DEVICE_ENGINE_SRC_LIST ${AMLOGICAL_SRC_LIST})
elseif(${ENABLE_ENGINE_TYPE} MATCHES "TRT")
    file(GLOB TRT_SRC_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "trtengine/*.cc")
    list(APPEND DEVICE_ENGINE_SRC_LIST ${TRT_SRC_LIST})
endif()

add_library(device_utile SHARED ${DEVICE_ENGINE_SRC_LIST})

if(${ENABLE_ENGINE_TYPE} MATCHES "NB")
    add_subdirectory(amlogicengine/vnn_common)
    target_link_libraries(device_utile amlogic_common)
endif()