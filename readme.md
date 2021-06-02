# <font size=6 color=red>Compile requirement</font>:
-  android-ndk >=r20
-  cmake >= 3.15
-  c++ Standard: C++14
=  GCC >= 5.4

# <font size=6 color=red>Build</font>:
  - git clone
```
git clone git@gitlab.alibaba-inc.com:cncvteam/refactor.git
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


# <font size=6 color=red>After build:</font>

  ### <font size=3>打包（含so和测试用例）输出路径</font>
  ```
${PROJECT_SOURCE_DIR}/output/    #you can find zip which contain all build output
  ```
  
  ### <font size=3>执行可执行用例test_st/test_ut</font>
  - cloud 
```
test_st --gtest_filter=*  #跑所有的test_st 测试用例
//test_st --gtest_filter=BenchMark.st # 指定执行BenchMark模块下的st用例
  ```  
  - device
```
1. move zip to device and unzip
    unzip Self_Architecture-0.1.1-Android.zip #(unzip on your_path) 
2. set LD_LIBRARY_PATH to link *.so
    export LD_LIBRARY_PATH=/your_path/Self_Architecture-0.1.1-Android/Framework-Pipeline/lib:$LD_LIBRARY_PATH
2. execute test
    test_st --gtest_filter=BenchMark.st  #跑所有的test_st 测试用例
```


# <font size=6 color=grenn>Commit code or push to origin</font>
   - 推送代码前请format code !
   ```
bash modules/scripts/format_source_code.sh -a # for format your code
   ```

# <font size=6 color=grenn>GOAL</font>: 
  - 2021/03/09 Primary goal:just create framework and test success, dataflow need to be test  
  - 2021/06/02 complele all feature and has test function well primary
