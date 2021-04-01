
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk_interface/include/service/ovx_inc/)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk_interface/include/service/ovx_inc/CL/)

include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk_interface/include/applib/ovxinc/include)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk_interface/include/applib/ovxinc/include/utils)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk_interface/include/applib/ovxinc/include/client)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/ddk_interface/include/applib/ovxinc/include/ops)

file(GLOB_RECURSE AMLOGIC_COMMON_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "*.c"
        #        "vnn_common/*.c"
        )

add_library(amlogic_common SHARED ${AMLOGIC_COMMON_LIST})
target_link_directories(amlogic_common PRIVATE ${PROJECT_SOURCE_DIR}/thirdparty/ddk_interface/lib/armeabi-v7a/)
target_link_libraries(amlogic_common jpeg ovxlib)