file(GLOB_RECURSE PARSE_SRC_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "*.cc"
    )

add_library(parseconfig SHARED ${PARSE_SRC_LIST})
