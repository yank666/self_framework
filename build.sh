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
  echo "    -k Enable make clean, clean up compilation generated cache "
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
  # Process the options
  while getopts 'drvj:t:b:e:k:' opt
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
          echo "=========testcase add build not support=========="
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
          if [[ "X$OPTARG" != "Xlinux" && "X$OPTARG" != "Xandriod" ]]; then
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
        *)
          echo "Unknown option ${opt}!"
          usage
          exit 1
    esac
  done
}

checkopts "$@"
echo "---------------- build start ----------------"
#  git submodule update --init
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
  CMAKE_ARGS="-DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DBUILD_PATH=$BUILD_PATH"
  CMAKE_ARGS="${CMAKE_ARGS} -DENABLE_ENGINE_TYPE=$ENABLE_ENGINE_TYPE"
  CMAKE_ARGS="${CMAKE_ARGS} -DENABLE_DEVICE=$DEVICE"
  echo "=================${CMAKE_ARGS}========================="
  cmake ${CMAKE_ARGS} ..
  cmake --build . ${CMAKE_VERBOSE} -j$THREAD_NUM
}


  if [[ "X$ENABLE_MAKE_CLEAN" = "Xon" ]]; then
    make_clean
  else
    build_project
  fi
  echo "----------------   build end   ----------------"