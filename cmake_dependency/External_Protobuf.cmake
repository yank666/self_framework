message("External project: protobuf")

SET(PROTO_INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparty/protobuf)
SET(PROTO_INCLUDE_DIR "${PROTO_INSTALL_DIR}/include" CACHE PATH "protobuf include directory." FORCE)
set(PROTO_LIBRARIES "${PROTO_INSTALL_DIR}/lib/libprotobuf.a" CACHE FILEPATH "protobuf libraries." FORCE)
message("============${PROTO_LIBRARIES}=============")
INCLUDE_DIRECTORIES(${PROTO_INCLUDE_DIR})

ExternalProject_Add (External_protobuf
        SOURCE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/protobuf/cmake
        CMAKE_ARGS          ${EXTRA_CMAKE_ARGS}
        CMAKE_ARGS          -DCMAKE_BUILD_TYPE=Release
                            -DCMAKE_INSTALL_PREFIX=${PROTO_INSTALL_DIR}
                            -Dprotobuf_BUILD_TESTS=OFF
                            -DCMAKE_POSITION_INDEPENDENT_CODE=ON
                            -DBUILD_SHARED_LIBS=OFF
                            -Dprotobuf_BUILD_PROTOC_BINARIES=OFF
        CMAKE_CACHE_ARGS   -DCMAKE_INSTALL_PREFIX:PATH=${PROTO_INSTALL_DIR}
                            -DCMAKE_BUILD_TYPE:STRING=Release
                            -Dprotobuf_BUILD_TESTS:BOOL=OFF
                            -DCMAKE_VERBOSE_MAKEFILE:BOOL=OFF
                            -DBUILD_SHARED_LIBS:BOOL=OFF
                            -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
                            -Dprotobuf_BUILD_PROTOC_BINARIES:BOOL=OFF
        )

ADD_LIBRARY(protobuf::libprotobuf STATIC IMPORTED GLOBAL)
SET_PROPERTY(TARGET protobuf::libprotobuf PROPERTY IMPORTED_LOCATION ${PROTO_LIBRARIES})
#[[if (${ENABLE_ENGINE_TYPE} MATCHES "NB")
#    target_link_libraries(protobuf::libprotobuf INTERFACE log)
#endif()]]
LIST(APPEND third_party_deps External_protobuf)
