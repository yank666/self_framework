message("External project: Gtest")

SET(GLOG_INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparty/gtest)
SET(GTEST_INSTALL_DIR   ${CMAKE_BINARY_DIR}/thirdparty/gtest)
SET(GTEST_INCLUDE_DIR "${GLOG_INSTALL_DIR}/include" CACHE PATH "gtest include directory." FORCE)
set(GTEST_LIBRARIES "${GTEST_INSTALL_DIR}/lib/libgtest.so" CACHE FILEPATH "gtest libraries." FORCE)

INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIR})

ExternalProject_Add (External_gtest
        SOURCE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/gtest
        CMAKE_ARGS      ${EXTRA_CMAKE_ARGS}
        CMAKE_ARGS      -DCMAKE_BUILD_TYPE=Release
                        -DCMAKE_INSTALL_PREFIX=${GTEST_INSTALL_DIR}
                        -DBUILD_TESTING=OFF
                        -DBUILD_SHARED_LIBS=ON
                        -DCMAKE_MACOSX_RPATH=TRUE
                        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
                        -Dgtest_disable_pthreads=ON
        CMAKE_CACHE_ARGS    -DCMAKE_INSTALL_PREFIX:PATH=${GTEST_INSTALL_DIR}
                            -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
                            -DBUILD_TESTING:BOOL=OFF
                            -DBUILD_SHARED_LIBS:BOOL=ON

        )

ADD_LIBRARY(gtest::gtest SHARED IMPORTED GLOBAL)
SET_PROPERTY(TARGET gtest::gtest PROPERTY IMPORTED_LOCATION ${GTEST_LIBRARIES})
LIST(APPEND third_party_deps External_gtest)