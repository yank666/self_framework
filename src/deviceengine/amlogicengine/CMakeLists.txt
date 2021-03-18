file(GLOB_RECURSE AMLOGIC_ENGINE_SRC_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "*.cc"
#        "vnn_common/*.c"
        )

add_library(amlogic_engine SHARED ${AMLOGIC_ENGINE_SRC_LIST})