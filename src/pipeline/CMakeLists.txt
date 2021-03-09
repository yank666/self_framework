file(GLOB_RECURSE PIPELINE_SRC_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        *.cc
        )
message("===========${PIPELINE_SRC_LIST}==================")
add_library(pipeline STATIC ${PIPELINE_SRC_LIST})