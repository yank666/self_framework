include (ExternalProject)

#ExternalProject_Add (External_glog
#        SOURCE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/glog
#        CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DCMAKE_POSITION_INDEPENDENT_CODE=ON
#        -DBUILD_SHARED_LIBS=ON -DWITH_GFLAGS=OFF
#        INSTALL_COMMAND ""
#        BINARY_DIR ${PROJECT_SOURCE_DIR}/build/thirdparty/glog
#        )
#add_library(glog::glog STATIC IMPORTED)
#set_target_properties(glog::glog PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/build/thirdparty/glog/libglog.so)
#add_dependencies(glog::glog External_glog)
#include_directories(${PROJECT_SOURCE_DIR}/build/thirdparty/glog)
#include_directories(${PROJECT_SOURCE_DIR}/thirdparty/glog/src)

ExternalProject_Add (OpenCV
        SOURCE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/opencv
        CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release -DWITH_PROTOBUF=OFF -DWITH_WEBP=OFF -DWITH_IPP=OFF -DWITH_ADE=OFF
        -DBUILD_ZLIB=ON  -DBUILD_JPEG=ON  -DBUILD_PNG=ON -DBUILD_OPENEXR=ON
        -DBUILD_TESTING=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_opencv_apps=OFF -DBUILD_opencv_python3=OFF
        -DBUILD_TIFF=OFF  -DWITH_JASPER=OFF  -DBUILD_JASPER=OFF
        INSTALL_COMMAND ""
        BINARY_DIR ${PROJECT_SOURCE_DIR}/build/thirdparty/opencv
        )


