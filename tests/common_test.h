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
  char *ReadFromFile(std::string file_name, size_t memlen);

  template <typename T>
  void PrintData(const std::string &name, T *output_data, int size);

  template <typename T>
  static int CompareOutputData(const T *output_data, const T *correct_data,
                               int size, float err_bound = 1e-4);
};

#endif  // SELF_ARCHITECTURE_COMMON_TEST_H
