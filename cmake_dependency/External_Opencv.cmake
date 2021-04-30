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
                        -DBUILD_JAVA=OFF
                        -DBUILD_DOCS=OFF
                        -DBUILD_ANDROID_EXAMPLES=OFF
                        -DBUILD_ANDROID_PROJECTS=OFF
                        -DBUILD_TESTS=OFF
                        -DBUILD_PERF_TESTS=OFF
                        -DBUILD_opencv_apps=OFF
                        -DBUILD_opencv_python2=OFF
                        -DBUILD_opencv_python3=OFF
                        -DWITH_CUDA=OFF
                        -DBUILD_SHARED_LIBS=ON
        CMAKE_CACHE_ARGS
                        -DCMAKE_INSTALL_PREFIX:PATH=${OPENCV_INSTALL_DIR}
                        -DCMAKE_BUILD_TYPE:STRING=Release
                        -DBUILD_JAVA:BOOL=OFF
                        -DBUILD_DOCS:BOOL=OFF
                        -DBUILD_ANDROID_EXAMPLES:BOOL=OFF
                        -DBUILD_ANDROID_PROJECTS:BOOL=OFF
                        -DBUILD_TESTS:BOOL=OFF
                        -DBUILD_PERF_TESTS:BOOL=OFF
                        -DBUILD_opencv_apps:BOOL=OFF
                        -DBUILD_opencv_python2:BOOL=OFF
                        -DBUILD_opencv_python3:BOOL=OFF
                        -DWITH_CUDA:BOOL=OFF
                        -DBUILD_SHARED_LIBS:BOOL=ON
        )

if (${ENABLE_ENGINE_TYPE} MATCHES "TRT")
    set(LIB_NAMES opencv_core opencv_highgui opencv_imgproc opencv_imgcodecs)
    foreach(LIB ${LIB_NAMES})
        set(OPENCV_LIBS ${OPENCV_LIBS} "${OPENCV_INSTALL_DIR}/lib/lib${LIB}.so")
    endforeach()
else()
    set(LIB_NAMES opencv_core opencv_highgui opencv_imgproc opencv_imgcodecs)
    foreach(LIB ${LIB_NAMES})
        set(OPENCV_LIBS ${OPENCV_LIBS} "${OPENCV_INSTALL_DIR}/sdk/native/libs/armeabi-v7a/lib${LIB}.so")
    endforeach()
endif ()


LIST(APPEND third_party_deps External_opencv)