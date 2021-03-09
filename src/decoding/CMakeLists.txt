#file(GLOB_RECURSE DECODE_SRC "*.cc")
set(DECODE_SRC yolov5_decoding.cc)
add_library(decode STATIC ${DECODE_SRC})
#target_link_libraries(decode Eigen3::Eigen)

add_subdirectory(../pipeline)