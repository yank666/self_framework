//
// Created by yankai.yan on 2021/5/6.
//

#include "common_test.h"

void CommonTest::SetUpTestCase() {}

void CommonTest::TearDownTestCase() {}

void CommonTest::SetUp() {}

void CommonTest::TearDown() {}

char *CommonTest::ReadFromFile(std::string file_name, size_t memlen) {
  std::ifstream fin(file_name, std::ios::binary);
  if (!fin.good()) {
    LOG(ERROR) << "file: " << file_name << " is not exist";
    return nullptr;
  }

  if (!fin.is_open()) {
    LOG(ERROR) << "Cannot open label file " << file_name;
    return nullptr;
  }
  fin.seekg(0, fin.end);
  size_t buflen = fin.tellg();
  if (buflen != memlen) {
    LOG(ERROR) << "Read file " << file_name
               << "failed, memory length is not excepted";
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

template <typename T>
void CommonTest::PrintData(const std::string &name, T *output_data, int size) {
  std::cout << "The " << name << " is as follows:" << std::endl;
  if (typeid(output_data[0]) == typeid(uint8_t) || typeid(output_data[0]) == typeid(int8_t)) {
    for (int i = 0; i < std::min(size, 100); i++) {
      std::cout << static_cast<int>(output_data[i]) << " ";
    }
  } else {
    for (int i = 0; i < std::min(size, 100); i++) {
      std::cout << output_data[i] << " ";
    }
  }
  std::cout << std::endl;
}

template <typename T>
int CommonTest::CompareOutputData(const T *output_data, const T *correct_data, int size, float err_bound) {
  float error = 0;
  for (int i = 0; i < size; i++) {
    T diff = std::fabs(output_data[i] - correct_data[i]);
    if (diff > 0.00001) {
      error += diff;
    }
  }
  error /= static_cast<float>(size);
  if (error > err_bound) {
    return 1;
  }
  return 0;
}