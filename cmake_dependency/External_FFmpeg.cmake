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
    set(ASM "${SYSROOT}/usr/include/arm-linux-androideabi")
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
            --enable-gpl
            --enable-jni
            --enable-small
            --enable-network
            --enable-mediacodec
            --enable-pic
            --disable-avdevice
            --disable-ffmpeg
            --disable-ffprobe
            --disable-asm
            --disable-avresample
            --disable-x86asm
            --disable-static
            --disable-neon
            --disable-hwaccels
            --disable-postproc
            --disable-doc
            --disable-symver
            --disable-stripping
            --cross-prefix=${CROSS_PREFIX}
            --target-os=android
            --arch=arm
            --cpu=${CPU}
            --cc=${CC}
            --cxx=${CXX}
            --enable-cross-compile
            --sysroot=${SYSROOT}
            --extra-cflags="-I$ASM -isysroot $SYSROOT -fPIC -D__thumb__ -mthumb -Wfatal-errors -Wno-deprecated -mfloat-abi=softfp -D__ANDROID_API__=28 -Os -marm"
            --extra-ldflags="-marm"
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

set(FFMPEG_LIB_NAMES avutil avformat avcodec avfilter swscale swresample)
foreach(LIB ${FFMPEG_LIB_NAMES})
    set(FFMPEG_LIBS ${FFMPEG_LIBS} "${FFMPEG_INSTALL_DIR}/lib/lib${LIB}.so")
endforeach()

LIST(APPEND third_party_deps External_ffmpeg)