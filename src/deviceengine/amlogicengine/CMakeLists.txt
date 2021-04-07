file(GLOB_RECURSE AMLOGIC_ENGINE_SRC_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "amlogical_engine_infer.cc"
        )
add_subdirectory(vnn_common)

add_library(amlogic_engine SHARED ${AMLOGIC_ENGINE_SRC_LIST})