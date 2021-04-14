file(GLOB_RECURSE PIPELINE_SRC_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "pipeline.cc"
        "decorator_stage/*.cc"
        )
add_subdirectory(deviceengine)
add_library(pipeline SHARED ${PIPELINE_SRC_LIST})
target_link_libraries(pipeline glog::glog)
target_link_libraries(pipeline device_utile)