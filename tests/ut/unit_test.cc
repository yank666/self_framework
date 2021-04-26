
#include "gtest/gtest.h"
#include <fstream>
#include <memory>
#include <cstdio>
#include "glog/logging.h"
#include "src/pipeline/deviceengine/abstractengine.h"
#include "src/pipeline/decorator_stage/yolo_decorator_stage.h"
#include "opencv2/opencv.hpp"
using namespace pipeline;
using namespace device;
class DecoratorUT : public testing::Test {
 public:
  DecoratorUT() = default;
  ~DecoratorUT() = default;
  static void SetUpTestCase(){};
  static void TearDownTestCase(){};
//  char *ReadFromFile(std::string file_name, size_t memlen);
  // every TEST_F macro will enter one
  virtual void SetUp(){};
  virtual void TearDown(){};
};

char *ReadFromFile(std::string file_name, size_t memlen) {
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

TEST_F(DecoratorUT, yolodecorator) {
  FLAGS_minloglevel = 0;
  char *in = nullptr;
  const uint32_t kInputSize = 333396;
  in = ReadFromFile("/home/yankai.yan/workbase/codeLib/refactor/bin/torch_out.bins", kInputSize * sizeof(float));
  ASSERT_NE(nullptr, in);
  contextPtr context_ptr = std::make_shared<Context>();
  std::vector<uint32_t> data_size_set = {kInputSize};
  context_ptr->out_shape_.resize(1);
  context_ptr->out_shape_[0] = data_size_set;
  context_ptr->out_dataflow_.resize(1);
  context_ptr->out_dataflow_[0].resize(kInputSize);
  memcpy(context_ptr->out_dataflow_[0].data(), in, kInputSize * sizeof(float));
  auto yolo_stage_ptr = std::make_shared<YoloDerocatestage>();
  yolo_stage_ptr->RunStage(context_ptr);
  LOG(INFO) << "Run SUCCESS!";
};

TEST_F(DecoratorUT, OpencvTest) {
  cv::Mat img;
  cv::imread("1.jpg");
  LOG(INFO) << "Run SUCCESS!";
}