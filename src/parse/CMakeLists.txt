file(GLOB PARSE_CONFIG_SRC_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "parse_config.cc"
    )
list(APPEND PARSE_CONFIG_SRC_LIST "${PROJECT_SOURCE_DIR}/modules/proto/${fil_we}.pb.cc")

add_library(parseconfig SHARED ${PARSE_CONFIG_SRC_LIST})
add_dependencies(parseconfig External_protobuf)
target_link_libraries(parseconfig protobuf::libprotobuf)
target_link_libraries(parseconfig glog::glog)

file(GLOB PARSE_VIDEO_SRC_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "parse_video.cc"
        )
add_library(parsevideo SHARED ${PARSE_VIDEO_SRC_LIST})
add_dependencies(parsevideo External_opencv)
target_link_libraries(parsevideo glog::glog ${OPENCV_LIBS})