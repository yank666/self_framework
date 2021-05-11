file(GLOB DEVICE_ENGINE_SRC_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "*.cc"
)

if(${ENABLE_ENGINE_TYPE} MATCHES "NB")
    file(GLOB AMLOGICAL_SRC_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "amlogicengine/*.cc")
    list(APPEND DEVICE_ENGINE_SRC_LIST ${AMLOGICAL_SRC_LIST})
    include_directories("../../../../../thirdparty/ddk")
    include_directories("../../../../../thirdparty/ddk/include")
    include_directories("../../../../../thirdparty/ddk/include/utils")
    include_directories("../../../../../thirdparty/ddk/include/client")
    include_directories("../../../../../thirdparty/ddk/include/ops")
    include_directories("../../../../../thirdparty/ddk/ovx_inc")
    include_directories("../../../../../thirdparty/ddk/ovx_inc/CL")
    file(GLOB_RECURSE AMLOGIC_COMMON_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
            "*.c"
            )
    list(APPEND DEVICE_ENGINE_SRC_LIST ${AMLOGIC_COMMON_LIST})
elseif(${ENABLE_ENGINE_TYPE} MATCHES "TRT")
    file(GLOB TRT_SRC_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "trtengine/*.cc")
    list(APPEND DEVICE_ENGINE_SRC_LIST ${TRT_SRC_LIST})
endif()

add_library(device_utile SHARED ${DEVICE_ENGINE_SRC_LIST})
add_dependencies(device_utile ${third_party_deps})
target_link_libraries(device_utile glog::glog ${OPENCV_LIBS})
if(${ENABLE_ENGINE_TYPE} MATCHES "NB")
    target_link_directories(device_utile PUBLIC ${PROJECT_SOURCE_DIR}/thirdparty/ddk/lib/armeabi-v7a)
    target_link_libraries(device_utile jpeg_amlogical_common ovxlib)
endif()