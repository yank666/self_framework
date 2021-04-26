message("External project: Opencv")

SET(OPENCV_INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparty/opnecv)
SET(OPENCV_INSTALL_DIR   ${CMAKE_BINARY_DIR}/thirdparty/opencv)
if (${ENABLE_ENGINE_TYPE} MATCHES "TRT")
    SET(OPENCV_INCLUDE_DIR "${OPENCV_INSTALL_DIR}/include" CACHE PATH "opencv include directory." FORCE)
    include_directories(${OPENCV_INCLUDE_DIR})
    set(OPENCV_CMAKE_ARGS_EXTRA "-DBUILD_FAT_JAVA_LIB=OFF")
    set(OPENCV_CMAKE_CACHE_ARGS_EXTRA "-DBUILD_FAT_JAVA_LIB:BOOL=OFF")
else()
    SET(OPENCV_INCLUDE_DIR "${OPENCV_INSTALL_DIR}/sdk/native/jni/include" CACHE PATH "opencv include directory." FORCE)
    include_directories(${OPENCV_INCLUDE_DIR})
    set(OPENCV_CMAKE_ARGS_EXTRA "-DBUILD_FAT_JAVA_LIB=ON")
    set(OPENCV_CMAKE_CACHE_ARGS_EXTRA "-DBUILD_FAT_JAVA_LIB:BOOL=ON")
endif()

ExternalProject_Add (External_opencv
        SOURCE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/opencv
        CMAKE_ARGS      ${EXTRA_CMAKE_ARGS}
        CMAKE_ARGS      -DCMAKE_BUILD_TYPE=Release
                        -DCMAKE_INSTALL_PREFIX=${OPENCV_INSTALL_DIR}
                        -DWITH_PROTOBUF=OFF
                        -DWITH_WEBP=OFF
                        -DWITH_IPP=OFF
                        -DWITH_ADE=OFF
                        -DBUILD_ZLIB=ON
                        -DBUILD_JPEG=ON
                        -DBUILD_PNG=ON
                        -DBUILD_OPENEXR=ON
                        -DBUILD_TESTS=OFF
                        -DBUILD_PERF_TESTS=OFF
                        -DBUILD_opencv_apps=OFF
                        -DCMAKE_SKIP_RPATH=TRUE
                        -DBUILD_opencv_python3=OFF
                        -DWITH_FFMPEG=OFF
                        -DWITH_TIFF=OFF
                        -DBUILD_TIFF=OFF
                        -DWITH_JASPER=OFF
                        -DBUILD_JASPER=OFF
                        -DWITH_CUDA=OFF
                        ${OPENCV_CMAKE_ARGS_EXTRA}
        CMAKE_CACHE_ARGS
                        -DCMAKE_INSTALL_PREFIX:PATH=${OPENCV_INSTALL_DIR}
                        -DCMAKE_BUILD_TYPE:STRING=Release
                        -DWITH_PROTOBUF:BOOL=OFF
                        -DWITH_WEBP:BOOL=OFF
                        -DWITH_IPP:BOOL=OFF
                        -DWITH_ADE:BOOL=OFF
                        -DBUILD_ZLIB:BOOL=ON
                        -DBUILD_JPEG:BOOL=ON
                        -DBUILD_PNG:BOOL=ON
                        -DBUILD_OPENEXR:BOOL=ON
                        -DBUILD_TESTS:BOOL=OFF
                        -DBUILD_PERF_TESTS:BOOL=OFF
                        -DBUILD_opencv_apps:BOOL=OFF
                        -DCMAKE_SKIP_RPATH:BOOL=TRUE
                        -DBUILD_opencv_python3:BOOL=OFF
                        -DWITH_FFMPEG:BOOL=OFF
                        -DWITH_TIFF:BOOL=OFF
                        -DBUILD_TIFF:BOOL=OFF
                        -DWITH_JASPER:BOOL=OFF
                        -DBUILD_JASPER:BOOL=OFF
                        -DWITH_CUDA:BOOL=OFF
                        ${OPENCV_CMAKE_CACHE_ARGS_EXTRA}
        )

if (${ENABLE_ENGINE_TYPE} MATCHES "TRT")
    set(LIB_NAMES opencv_core opencv_highgui opencv_imgproc opencv_imgcodecs)
    foreach(LIB ${LIB_NAMES})
        set(OPENCV_LIBS ${OPENCV_LIBS} "${OPENCV_INSTALL_DIR}/lib/lib${LIB}.so")
    endforeach()
else()
    set(LIB_NAMES opencv_java3)
    foreach(LIB ${LIB_NAMES})
        set(OPENCV_LIBS ${OPENCV_LIBS} "${OPENCV_INSTALL_DIR}/sdk/native/libs/armeabi-v7a/lib${LIB}.so")
    endforeach()
endif ()


LIST(APPEND third_party_deps External_opencv)