file(GLOB PARSE_CONFIG_SRC_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "*.cc"
    )
list(APPEND PARSE_CONFIG_SRC_LIST "${PROJECT_SOURCE_DIR}/modules/proto/${fil_we}.pb.cc")

add_library(parsemodule SHARED ${PARSE_CONFIG_SRC_LIST})
add_dependencies(parsemodule ${third_party_deps})
target_link_libraries(parsemodule protobuf::libprotobuf ${OPENCV_LIBS} ${FFMPEG_LIBS})
target_link_libraries(parsemodule glog::glog)


