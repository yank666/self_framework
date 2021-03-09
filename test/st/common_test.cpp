//
// Created by yankai on 2021/1/28.
//
#include "gtest/gtest.h"
#include <fstream>
#include "glog/logging.h"

class BenchMark : public testing::Test{
public:
  BenchMark() = default;
  ~BenchMark()= default;
  static void SetUpTestCase(){};
  static void TearDownTestCase(){};
  char* ReadFromFile(std::string file_name, size_t memlen);
  // every TEST_F macro will enter one
  virtual void SetUp(){};
  virtual void TearDown(){};
};

char* BenchMark::ReadFromFile(std::string file_name, size_t memlen) {
  std::ifstream fin(file_name, std::ios::binary);
  if (!fin.good()) {
    LOG(ERROR) << "file: " << file_name << " is not exist";
    return nullptr;
  }
  
  if (!fin.is_open()) {
    LOG(ERROR) << "Cannot open label file " << file_name;
    return nullptr;
  }
  fin.seekg (0, fin.end);
  size_t buflen  = fin.tellg();
  if (buflen != memlen) {
    LOG(ERROR) << "Read file " << file_name << "failed, memory length is not excepted";
    return nullptr;
  }
  char *buf = new (std::nothrow) char[buflen];
  if (buf == nullptr) {
    LOG(ERROR) << "Cannot malloc mem" << file_name;
    fin.close();
    return nullptr;
  }
  fin.seekg(0, std::ios::beg);
  fin.read(buf, buflen);
  LOG(INFO) << "Read file " << file_name << "success!";
  return buf;
}
TEST_F(BenchMark, Performance_test) {
  float *input0 = reinterpret_cast<float *>(
      ReadFromFile("D:\\workBase\\Goodlib\\software_constauct_base\\bin\\input1.bin",
          1*3*252*9* sizeof(float)));
  float *input1 = reinterpret_cast<float *>(
      ReadFromFile("D:\\workBase\\Goodlib\\software_constauct_base\\bin\\input2.bin",
      1*3*1008*9* sizeof(float)));
  float *input2 = reinterpret_cast<float *>(
      ReadFromFile("D:\\workBase\\Goodlib\\software_constauct_base\\bin\\input3.bin",
      1*3*4032*9* sizeof(float)));
  float* inputArray[] = {input0, input1, input2};

  
  delete []input0;
  delete []input1;
  delete []input2;
}
TEST_F(BenchMark, Accuracy_test) {

}
