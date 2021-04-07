file(GLOB_RECURSE PARSE_SRC_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "*.cc"
    )

add_library(parseconfig SHARED ${PARSE_SRC_LIST})
list(APPEND srcs_var "${PROJECT_SOURCE_DIR}/modules/proto/${fil_we}.pb.cc")
add_library(proto_OBJS SHARED ${srcs_var})
target_link_libraries(proto_OBJS protobuf::libprotobuf)
target_link_libraries(parseconfig proto_OBJS glog::glog)