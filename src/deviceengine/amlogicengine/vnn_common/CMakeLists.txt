include_directories("${PROJECT_SOURCE_DIR}/thirdparty/ddk")
include_directories("${PROJECT_SOURCE_DIR}/thirdparty/ddk/include")
include_directories("${PROJECT_SOURCE_DIR}/thirdparty/ddk/include/utils")
include_directories("${PROJECT_SOURCE_DIR}/thirdparty/ddk/include/client")
include_directories("${PROJECT_SOURCE_DIR}/thirdparty/ddk/include/ops")
include_directories("${PROJECT_SOURCE_DIR}/thirdparty/ddk/ovx_inc")
include_directories("${PROJECT_SOURCE_DIR}/thirdparty/ddk/ovx_inc/CL")


message("=================${PROJECT_SOURCE_DIR}/thirdparty/ddk/======================")
file(GLOB_RECURSE AMLOGIC_COMMON_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "*.c"
        )

add_library(amlogic_common SHARED ${AMLOGIC_COMMON_LIST})
target_link_directories(amlogic_common PUBLIC ${PROJECT_SOURCE_DIR}/thirdparty/ddk/lib/armeabi-v7a/)
target_link_libraries(amlogic_common jpeg ovxlib)