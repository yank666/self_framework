
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TEST_SOURCE_FILE ${TEST_DIR}/test_main.cc)

file(GLOB_RECURSE ST_SOURCE_FILE ${TEST_DIR}/st/*.cc)
list(APPEND ST_SOURCE_FILE ${TEST_SOURCE_FILE})

file(GLOB_RECURSE UT_SOURCE_FILE ${TEST_DIR}/ut/*.cc)
list(APPEND UT_SOURCE_FILE ${TEST_SOURCE_FILE})

add_executable(test_st ${ST_SOURCE_FILE})
target_link_libraries(test_st gtest::gtest glog::glog)
target_link_libraries(test_st parseconfig pipeline device_utile)


add_executable(test_ut ${UT_SOURCE_FILE})
add_dependencies(test_ut ${third_party_deps})
target_link_libraries(test_ut gtest::gtest glog::glog ${OPENCV_LIBS})
target_link_libraries(test_ut parseconfig pipeline device_utile)
