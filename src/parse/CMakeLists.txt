file(GLOB_RECURSE PARSE_SRC_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "*.cc"
    )
list(APPEND PARSE_SRC_LIST "${PROJECT_SOURCE_DIR}/modules/proto/${fil_we}.pb.cc")

add_library(parseconfig SHARED ${PARSE_SRC_LIST})
target_link_libraries(parseconfig protobuf::libprotobuf)
target_link_libraries(parseconfig glog::glog)