Compile requirement:
  android-ndk >=r20
  cmake >= 3.15
  c++ Standard: C++14
  GCC >= 5.4

Build:
  
  '''
  git clone git@gitlab.alibaba-inc.com:yankai.yan/refactor.git
  '''
  
  '''
  export ANDROID_NDK=${your_android_ndk_path}
  '''
  //for example: export ANDROID_NDK=/home/yankai.yan/tools/android-ndk-r20b
  
  '''
  bash build.sh -b trt -j30 
  '''
  // for trt model
  
  '''
  bash build.sh  -b nb -j30 
  '''
  // for amlogical model
  
  '''
  bash build.sh -b nb -j30 -t on 
  '''
  // add compile test
  
  '''
  bash build.sh -p on -j30 
  '''
  // if you want to update *.pb.cc  

after build:
  ${PROJECT_SOURCE_DIR}/output/    #you can find zip which contain all build output

add:
   bash modules/scripts/format_source_code.sh  // for format your code

for clion IDE compile option
'''
  trt: -DCMAKE_BUILD_TYPE=Debug -DENABLE_ENGINE_TYPE=NB
'''  

2021/03/09
 Primary goal:just create framework and test success, dataflow need to be test
