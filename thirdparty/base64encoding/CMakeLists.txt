file(GLOB_RECURSE BASE64_SRC_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "*.cpp"
        )
add_library(base64 SHARED ${BASE64_SRC_LIST})