//
// Created by yankai.yan on 2021/5/6.
//

#ifndef SELF_ARCHITECTURE_COMMON_TEST_H
#define SELF_ARCHITECTURE_COMMON_TEST_H
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include "gtest/gtest.h"

class CommonTest : public testing::Test {
 public:
  // TestCase only enter once
  static void SetUpTestCase();
  static void TearDownTestCase();

  // every TEST_F macro will enter one
  virtual void SetUp();
  virtual void TearDown();
  static char *ReadFromFile(std::string file_name, size_t memlen);
  static int SaveToFile(const std::string &filename, char *buf, uint32_t fb_size);
  template <typename T>
  void PrintData(const std::string &name, T *output_data, int size);

  template <typename T>
  static int CompareOutputData(const T *output_data, const T *correct_data,
                               int size, float err_bound = 1e-4);
  static void GetFileNames(std::string path, const std::string& ext_name,
                           std::vector<std::string> *filenames);
};

#endif  // SELF_ARCHITECTURE_COMMON_TEST_H
