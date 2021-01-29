//
// Created by yankai on 2021/1/28.
//
#include "gtest/gtest.h"
class BenchMark : public testing::Test{
public:
  BenchMark() = default;
  ~BenchMark()= default;
  static void SetUpTestCase(){};
  static void TearDownTestCase(){};
  
  // every TEST_F macro will enter one
  virtual void SetUp(){};
  virtual void TearDown(){};
};

TEST_F(BenchMark, simply) {
  int x = 1;
  int y = 1;
  std::cout <<"=====================";
  ASSERT_EQ(x,y);
}