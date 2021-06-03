message("External project: ffmpeg")


SET(FFMPEG_INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparty/ffmpeg)
SET(FFMPEG_INCLUDE_DIR "${FFMPEG_INSTALL_DIR}/include" CACHE PATH "ffmpeg include directory." FORCE)
INCLUDE_DIRECTORIES(${FFMPEG_INCLUDE_DIR})
if (${ENABLE_ENGINE_TYPE} MATCHES "NB")
    set(API 28)
    set(ARCH "arm")
    set(CPU "armv7-a")
    set(OS "android")
    set(TOOLCHAIN "${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64")
    set(SYSROOT "${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot")
    set(CC "${TOOLCHAIN}/bin/armv7a-linux-androideabi${API}-clang")
    set(CXX "${TOOLCHAIN}/bin/armv7a-linux-androideabi${API}-clang++")
    set(STRIP "${TOOLCHAIN}/bin/arm-linux-androideabi-strip")
    set(CROSS_PREFIX "${TOOLCHAIN}/bin/arm-linux-androideabi-")
    SET(OPTIMIZE_CFLAGS "-march=${CPU}")

    ExternalProject_Add (External_ffmpeg
            SOURCE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/ffmpeg
            CMAKE_ARGS      ${EXTRA_CMAKE_ARGS}
            CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${FFMPEG_INSTALL_DIR}
            --enable-shared
            --disable-static
            --disable-neon
            --disable-hwaccels
            --disable-gpl
            --disable-postproc
            --disable-doc
            --disable-ffmpeg
            --disable-ffprobe
            --enable-ffplay
            --disable-doc
            --disable-symver
            --enable-pic
            --disable-postproc
            --cross-prefix=${CROSS_PREFIX}
            --target-os=android
            --arch=arm
            --cpu=${CPU}
            --cc=${CC}
            --cxx=${CXX}
            --strip=${STRIP}
            --enable-cross-compile
            --sysroot=${SYSROOT}
            --extra-cflags="-Os -fpic -fno-rtti ${OPTIMIZE_CFLAGS}"
        )
else()
    ExternalProject_Add (External_ffmpeg
            SOURCE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/ffmpeg
            CMAKE_ARGS      ${EXTRA_CMAKE_ARGS}
            CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${FFMPEG_INSTALL_DIR}
            --enable-ffplay
            --enable-pic
            --enable-shared
            --disable-x86asm
            --disable-static
            --disable-neon
            --disable-hwaccels
            --disable-gpl
            --disable-doc
            --disable-doc
            --disable-symver
            --disable-postproc
            --extra-cflags="-Os -fpic -fno-rtti"
            )
endif()

set(FFMPEG_LIB_NAMES avcodec avdevice avfilter avformat avutil swresample swscale)
foreach(LIB ${FFMPEG_LIB_NAMES})
    set(FFMPEG_LIBS ${FFMPEG_LIBS} "${FFMPEG_INSTALL_DIR}/lib/lib${LIB}.so")
endforeach()

LIST(APPEND third_party_deps External_ffmpeg)