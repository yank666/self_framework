# Compile requirement:
-  android-ndk >=r20
-  cmake >= 3.15
-  c++ Standard: C++14
=  GCC >= 5.4

# Build:
  - git clone
```
    git clone git@gitlab.alibaba-inc.com:yankai.yan/refactor.git
```

  - 设置android-ndk 路径//for example: export ANDROID_NDK=/home/yourhome_path/tools/android-ndk-r20b
```
   export ANDROID_NDK=${your_android_ndk_path}
```

  - 编译 (编译选项请看 bash build.sh --help)
```
    bash build.sh -b trt -j30  # for trt model
    // bash build.sh  -b nb -j30  # for amlogical model
```

  - 添加编译test模块
```
    bash build.sh -b nb -j30 -t on 
```

  - 重新生成protoc,更新修改的*.pb.cc 和*.pb.h
  ```
  bash build.sh -p on -j30 
  ```


# After build
  - 打包（含so和测试用例）输出路径
  ```
  ${PROJECT_SOURCE_DIR}/output/    #you can find zip which contain all build output
  ```
  
  - 执行可执行用例test_st/test_ut
  ```
  test_st --gtest_filter=*  #跑所有的test_st 测试用例
  //test_st --gtest_filter=BenchMark.st # 指定执行BenchMark模块下的st用例
  ```
  
# Commit code or push to origin
   - 推送代码前请format code !
   ```
   bash modules/scripts/format_source_code.sh -a # for format your code
   ```
  
# GOAL: 2021/03/09
  - Primary goal:just create framework and test success, dataflow need to be test
