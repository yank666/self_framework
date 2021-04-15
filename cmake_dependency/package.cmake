##if you need create tar.gz ,you can change 'ZIP' to 'TGZ"##
set(RUNTIME_PKG_NAME "Framework-Pipeline")
set(RUNTIME_COMPONENT_NAME "runtime_component")
set(RUNTIME_BIN_DIR ${RUNTIME_PKG_NAME}/bin)
set(RUNTIME_LIB_DIR ${RUNTIME_PKG_NAME}/lib)
set(RUNTIME_INC_DIR ${RUNTIME_PKG_NAME}/include)
install(FILES ${PROJECT_SOURCE_DIR}/src/pipeline/pipeline.h DESTINATION ${RUNTIME_INC_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})

install(FILES ${CMAKE_BINARY_DIR}/thirdparty/glog/libglog.so
        DESTINATION ${RUNTIME_LIB_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
install(FILES ${CMAKE_BINARY_DIR}/thirdparty/protobuf/cmake/libprotobuf.a
        DESTINATION ${RUNTIME_LIB_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
install(FILES ${CMAKE_BINARY_DIR}/src/pipeline/libpipeline.so
        DESTINATION ${RUNTIME_LIB_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
install(FILES ${CMAKE_BINARY_DIR}/src/parse/libparseconfig.so
        DESTINATION ${RUNTIME_LIB_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
install(FILES ${CMAKE_BINARY_DIR}/src/parse/libproto_OBJS.so
        DESTINATION ${RUNTIME_LIB_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
install(FILES ${CMAKE_BINARY_DIR}/src/pipeline/deviceengine/libdevice_utile.so
        DESTINATION ${RUNTIME_LIB_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
if(${ENABLE_ENGINE_TYPE} MATCHES "NB")
    install(FILES ${CMAKE_BINARY_DIR}/src/pipeline/deviceengine/amlogicengine/vnn_common/libamlogic_common.so
            DESTINATION ${RUNTIME_LIB_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
    install(FILES ${PROJECT_SOURCE_DIR}/thirdparty/ddk/lib/armeabi-v7a/libjpeg.so
            DESTINATION ${RUNTIME_LIB_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
    install(FILES ${PROJECT_SOURCE_DIR}/thirdparty/ddk/lib/armeabi-v7a/libovxlib.so
            DESTINATION ${RUNTIME_LIB_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
    install(FILES ${ANDROID_NDK}/sources/cxx-stl/llvm-libc++/libs/armeabi-v7a/libc++_shared.so
            DESTINATION ${RUNTIME_LIB_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
elseif(${ENABLE_ENGINE_TYPE} MATCHES "TRT")
     message("==========ERROR: please add trt devices so============")
endif()


if(BUILD_TESTCASE)
    install(FILES ${CMAKE_BINARY_DIR}/lib/libgtest.so
            DESTINATION ${RUNTIME_LIB_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
    install(TARGETS test_st RUNTIME
            DESTINATION ${RUNTIME_BIN_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
    install(FILES ${PROJECT_SOURCE_DIR}/modules/models.cfg
            DESTINATION ${RUNTIME_BIN_DIR} COMPONENT ${RUNTIME_COMPONENT_NAME})
endif()

set(CPACK_GENERATOR ZIP)
set(CPACK_COMPONENTS_ALL ${RUNTIME_COMPONENT_NAME})
set(CPACK_PACKAGE_DIRECTORY ${PROJECT_SOURCE_DIR}/output)

include(CPack)