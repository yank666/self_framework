set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--rpath='$ORIGIN:$ORIGIN/lib:$ORIGIN/../lib'")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--rpath='$ORIGIN:$ORIGIN/lib:$ORIGIN/../lib'")
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB TEST_SOURCE_FILE  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "test_main.cc"
        "common_test.cc"
        )

file(GLOB_RECURSE UT_SOURCE_FILE ${TEST_DIR}/ut/*.cc)
list(APPEND UT_SOURCE_FILE ${TEST_SOURCE_FILE} ${TEST_SOURCE_FILE})

add_executable(test_ut ${UT_SOURCE_FILE})
add_dependencies(test_ut ${third_party_deps})
target_link_libraries(test_ut ${OPENCV_LIBS})
target_link_libraries(test_ut gtest::gtest glog::glog)
target_link_libraries(test_ut parsemodule pipeline device_utile)
target_link_libraries(test_ut MOT_Tracker)




file(GLOB_RECURSE ST_SOURCE_FILE ${TEST_DIR}/st/*.cc)
list(APPEND ST_SOURCE_FILE ${TEST_SOURCE_FILE} ${TEST_SOURCE_FILE})

add_executable(test_st ${ST_SOURCE_FILE})
add_dependencies(test_st ${third_party_deps})
target_link_libraries(test_st ${OPENCV_LIBS})
target_link_libraries(test_st gtest::gtest glog::glog)
target_link_libraries(test_st parsemodule pipeline device_utile)