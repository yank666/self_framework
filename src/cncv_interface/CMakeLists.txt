file(GLOB PARSE_CONFIG_SRC_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "*.cc"
        )
add_library(cncv_person_detect SHARED ${PARSE_CONFIG_SRC_LIST})
target_link_libraries(cncv_person_detect pipeline)
target_link_libraries(cncv_person_detect ${OPENCV_LIBS})