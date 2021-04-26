message("External project: Glog")

SET(GLOG_SOURCES_DIR ${THIRD_PARTY_PATH}/glog)
SET(GLOG_INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparty/glog)
SET(GLOG_INCLUDE_DIR "${GLOG_INSTALL_DIR}/include" CACHE PATH "glog include directory." FORCE)

SET(GLOG_LIBRARIES "${GLOG_INSTALL_DIR}/lib/libglog.so" CACHE FILEPATH "glog library." FORCE)
INCLUDE_DIRECTORIES(${GLOG_INCLUDE_DIR})

ExternalProject_Add (External_glog
        SOURCE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/glog
        CMAKE_ARGS      ${EXTRA_CMAKE_ARGS}
        CMAKE_ARGS      -DCMAKE_INSTALL_PREFIX=${GLOG_INSTALL_DIR}
                        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
                        -DWITH_GFLAGS=ON
                        -DBUILD_TESTING=OFF
                        -DCMAKE_BUILD_TYPE=Release
        CMAKE_CACHE_ARGS    -DCMAKE_INSTALL_PREFIX:PATH=${GLOG_INSTALL_DIR}
                            -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
                            -DBUILD_SHARED_LIBS:BOOL=ON
                            -DWITH_GFLAGS:BOOL=OFF
                            -DCMAKE_BUILD_TYPE:STRING=Release
        )

ADD_LIBRARY(glog::glog SHARED IMPORTED GLOBAL)
SET_PROPERTY(TARGET glog::glog PROPERTY IMPORTED_LOCATION ${GLOG_LIBRARIES})
LIST(APPEND third_party_deps External_glog)
