##if you need create tar.gz ,you can change 'ZIP' to 'TGZ"##
set(RUNTIME_PKG_NAME "Framework-Pipeline")
set(RUNTIME_COMPONENT_NAME "runtime_component")
set(RUNTIME_BIN_DIR ${RUNTIME_PKG_NAME}/bin)
set(RUNTIME_LIB_DIR ${RUNTIME_PKG_NAME}/lib)
set(RUNTIME_INC_DIR ${RUNTIME_PKG_NAME}/include)
install(FILES ${PROJECT_SOURCE_DIR}/src/cncv_interface/cncv_persondetect_interface.h DESTINATION ${RUNTIME_INC_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})

install(FILES ${CMAKE_BINARY_DIR}/src/cncv_interface/libcncv_person_detect.so
        DESTINATION ${RUNTIME_LIB_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
install(FILES ${CMAKE_BINARY_DIR}/thirdparty/glog/lib/libglog.so
        DESTINATION ${RUNTIME_LIB_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
install(FILES ${CMAKE_BINARY_DIR}/thirdparty/protobuf/lib/libprotobuf.a
        DESTINATION ${RUNTIME_LIB_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
install(FILES ${CMAKE_BINARY_DIR}/thirdparty/base64encoding/libbase64.so
        DESTINATION ${RUNTIME_LIB_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
install(FILES ${CMAKE_BINARY_DIR}/src/pipeline/libpipeline.so
        DESTINATION ${RUNTIME_LIB_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
install(FILES ${CMAKE_BINARY_DIR}/src/parse/libparsemodule.so
        DESTINATION ${RUNTIME_LIB_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
install(FILES ${CMAKE_BINARY_DIR}/src/pipeline/deviceengine/libdevice_utile.so
        DESTINATION ${RUNTIME_LIB_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
install(FILES ${CMAKE_BINARY_DIR}/thirdparty/tracker/libMOT_Tracker.so
        DESTINATION ${RUNTIME_LIB_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})

if(${ENABLE_ENGINE_TYPE} MATCHES "NB")
#    install(DIRECTORY ${CMAKE_BINARY_DIR}/thirdparty/opencv/sdk/native/jni/include/ DESTINATION ${RUNTIME_INC_DIR}
#            COMPONENT ${RUNTIME_COMPONENT_NAME} FILES_MATCHING PATTERN "*.h")
    install(DIRECTORY ${CMAKE_BINARY_DIR}/thirdparty/opencv/sdk/native/libs/armeabi-v7a/ DESTINATION ${RUNTIME_LIB_DIR}
            COMPONENT ${RUNTIME_COMPONENT_NAME} FILES_MATCHING PATTERN "*.so")
    install(DIRECTORY ${CMAKE_BINARY_DIR}/thirdparty/ffmpeg/lib/ DESTINATION ${RUNTIME_LIB_DIR}
            COMPONENT ${RUNTIME_COMPONENT_NAME} FILES_MATCHING PATTERN "*.so")
    install(FILES ${PROJECT_SOURCE_DIR}/thirdparty/ddk/lib/armeabi-v7a/libjpeg_amlogical_common.so
            DESTINATION ${RUNTIME_LIB_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
    install(FILES ${PROJECT_SOURCE_DIR}/thirdparty/ddk/lib/armeabi-v7a/libovxlib.so
            DESTINATION ${RUNTIME_LIB_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
    install(FILES ${ANDROID_NDK}/sources/cxx-stl/llvm-libc++/libs/armeabi-v7a/libc++_shared.so
            DESTINATION ${RUNTIME_LIB_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
elseif(${ENABLE_ENGINE_TYPE} MATCHES "TRT")
    install(DIRECTORY ${CMAKE_BINARY_DIR}/thirdparty/opencv/include DESTINATION ${RUNTIME_INC_DIR}
            COMPONENT ${RUNTIME_COMPONENT_NAME} FILES_MATCHING PATTERN "*.h" PATTERN)
    install(DIRECTORY ${CMAKE_BINARY_DIR}/thirdparty/opencv/lib/ DESTINATION ${RUNTIME_LIB_DIR}
            COMPONENT ${RUNTIME_COMPONENT_NAME} FILES_MATCHING PATTERN "*.so.*" PATTERN)
endif()

if(BUILD_TESTCASE)
    install(FILES ${CMAKE_BINARY_DIR}/thirdparty/gtest/lib/libgtest.so
            DESTINATION ${RUNTIME_LIB_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
    install(TARGETS test_st RUNTIME
            DESTINATION ${RUNTIME_BIN_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
    install(TARGETS test_ut RUNTIME
            DESTINATION ${RUNTIME_BIN_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
    install(FILES ${PROJECT_SOURCE_DIR}/modules/config_file/models.cfg
            DESTINATION ${RUNTIME_BIN_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
    install(FILES ${PROJECT_SOURCE_DIR}/modules/config_file/models.cfg
            DESTINATION ${RUNTIME_BIN_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
    install(FILES ${PROJECT_SOURCE_DIR}/modules/config_file/config_person_mobile.json
            DESTINATION ${RUNTIME_BIN_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
endif()

set(CPACK_GENERATOR ZIP)
set(CPACK_COMPONENTS_ALL ${RUNTIME_COMPONENT_NAME})
set(CPACK_PACKAGE_DIRECTORY ${PROJECT_SOURCE_DIR}/output)

include(CPack)