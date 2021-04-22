#!/bin/bash

set -e
BASEPATH=$(cd "$(dirname $0)"; pwd)
CUDA_PATH=""
export BUILD_PATH="${BASEPATH}/build/"
usage()
{
  echo "Usage:"
  echo "bash build.sh [-d] [-r] [-v] [-j[n]] [-t ut|st]  [-b nb|trt]"
  echo ""
  echo "Example bash build.sh -r -j8 -b nb"
  echo "Options:"
  echo "    -d Debug mode"
  echo "    -r Release mode, default mode"
  echo "    -v Display build command"
  echo "    -j[n] Set the threads when building (Default: -j8)"
  echo "    -t Run testcases, default off"
  echo "    -b Select engine backend, available: trt(GPU) or nb(Amlogic)"
  echo "    -e Select compile runtime backend, linux or andriod"
  echo "    -k [on/off] Enable make clean, clean up compilation generated cache "
  echo "    -p [on/off] to update new *.pb.cc and *.pb.h, Enable compile protoc"
}

check_on_off()
{
  if [[ "X$1" != "Xon" && "X$1" != "Xoff" ]]; then
    echo "Invalid value $1 for option -$2"
    usage
    exit 1
  fi
}

checkopts()
{
  # Init default values of build options
  THREAD_NUM=8
  DEBUG_MODE="off"
  VERBOSE=""
  RUN_TESTCASES="off"
  USE_GLOG="on"
  ENABLE_VERBOSE="off"
  ANDROID_STL="c++_shared"
  ENABLE_MAKE_CLEAN="off"
  ENABLE_ENGINE_TYPE="nb"
  DEVICE="linux"
  ENABLE_MAKE_CLEAN="off"
  ENABLE_PROTOC="off"
  # Process the options
  while getopts 'drvj:t:b:e:k:p:' opt
  do
    OPTARG=$(echo ${OPTARG} | tr '[A-Z]' '[a-z]')
      case "${opt}" in
        d)
          DEBUG_MODE="on"
          ;;
        r)
          DEBUG_MODE="off"
          ;;
        j)
          THREAD_NUM=$OPTARG
          ;;
        t)
          if [[ "X$OPTARG" != "Xon" && "X$OPTARG" != "Xoff" ]]; then
            echo "Invalid value ${OPTARG} for option -t"
            usage
            exit 1
          fi
          RUN_TESTCASES=$(echo "$OPTARG" | tr '[a-z]' '[A-Z]')
          ;;
        b)
          if [[ "X$OPTARG" != "Xtrt" && "X$OPTARG" != "Xnb" ]]; then
            echo "Invalid value ${OPTARG} for option -b"
            usage
            exit 1
          fi
          ENABLE_ENGINE_TYPE=$(echo "$OPTARG" | tr '[a-z]' '[A-Z]')
          ;;
        e)
          if [[ "X$OPTARG" != "Xlinux" && "X$OPTARG" != "Xandroid" ]]; then
            echo "Invalid value ${OPTARG} for option -e"
            usage
            exit 1
          fi
          DEVICE=$(echo "$OPTARG" | tr '[a-z]' '[A-Z]')
          ;;
        k)
          check_on_off $OPTARG k
          ENABLE_MAKE_CLEAN="$OPTARG"
          echo "enable make clean"
          ;;
        p)
          check_on_off $OPTARG p
          ENABLE_PROTOC="$OPTARG"
          echo "enable compile protoc"
          ;;
        *)
          echo "Unknown option ${opt}!"
          usage
          exit 1
    esac
  done
}

checkndk() {
    if [ "${ANDROID_NDK}" ]; then
        echo -e "\e[31mANDROID_NDK_PATH=$ANDROID_NDK  \e[0m"
    else
        echo -e "\e[31mplease set ANDROID_NDK in environment variable for example: export ANDROID_NDK=/root/usr/android-ndk-r20b/ \e[0m"
        exit 1
    fi
}

make_clean()
{
  echo "enbale make clean"
  cd "${BUILD_PATH}/"
  cmake --build . --target clean
}

build_project()
{
  mkdir -pv "${BUILD_PATH}/"
  cd "${BUILD_PATH}/"
    BUILD_TYPE="Release"
  if [[ "${DEBUG_MODE}" == "on" ]]; then
    BUILD_TYPE="Debug"
  fi
  CMAKE_ARGS="-DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DBUILD_PATH=$BUILD_PATH -DBUILD_TESTCASE=${RUN_TESTCASES}"
  CMAKE_ARGS="${CMAKE_ARGS} -DENABLE_ENGINE_TYPE=$ENABLE_ENGINE_TYPE"
  if [[ "${ENABLE_ENGINE_TYPE}" == "NB" ]]; then
    checkndk
    cmake -DCMAKE_TOOLCHAIN_FILE="${ANDROID_NDK}/build/cmake/android.toolchain.cmake" -DANDROID_NATIVE_API_LEVEL="28" \
    -DANDROID_NDK="${ANDROID_NDK}" -DANDROID_ABI="armeabi-v7a" -DANDROID_TOOLCHAIN_NAME="aarch64-linux-android-clang" \
    -DANDROID_STL=${ANDROID_STL} -DCMAKE_BUILD_TYPE=${BUILD_TYPE}  -DENABLE_ENGINE_TYPE="$ENABLE_ENGINE_TYPE" \
    -DBUILD_TESTCASE=${RUN_TESTCASES} ..
     make -j$THREAD_NUM
  else
     echo "=================${CMAKE_ARGS}========================="
     cmake ${CMAKE_ARGS} ..
     cmake --build . ${CMAKE_VERBOSE} -j$THREAD_NUM
  fi

}

checkopts "$@"
echo "---------------- build start ----------------"
 git submodule update --init
  if [[ "X$ENABLE_PROTOC" = "Xon" ]]; then
    cd cmake_dependency/
    mkdir -pv "build_dependency/"
    cd build_dependency
    cmake -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_BUILD_SHARED_LIBS=OFF -Dbase_path="${BASEPATH}" ..
    make -j$THREAD_NUM
    exit 0
  fi
  if [[ "X$ENABLE_MAKE_CLEAN" = "Xon" ]]; then
    make_clean
  elif [[ "X$ENABLE_MAKE_CLEAN" = "Xoff" ]]; then
    build_project
  fi
  echo "----------------   build end   ----------------"
