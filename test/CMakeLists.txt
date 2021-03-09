option(ENABLE_ST "default include test st" ON)

set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TEST_SOURCE_FILE ${TEST_DIR}/test_main.cpp)
if(ENABLE_ST)
    file(GLOB_RECURSE ST_SOURCE_FILE ${TEST_DIR}/st/*.cpp)
    set(TEST_SOURCE_FILE ${TEST_SOURCE_FILE} ${ST_SOURCE_FILE})
endif()

enable_testing()
add_executable(test_st ${TEST_SOURCE_FILE})
target_link_libraries(test_st gtest glog::glog)