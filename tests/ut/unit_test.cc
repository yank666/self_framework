
#include "common_test.h"
#include <fstream>

#include "glog/logging.h"
#include "gtest/gtest.h"
#include "src/pipeline/deviceengine/abstractengine.h"
#include "src/pipeline/decorator_stage/yolo_decorator_stage.h"
#include "opencv2/opencv.hpp"

using namespace pipeline;
using namespace device;

class Unit : public CommonTest  {
 public:
  Unit() = default;
  ~Unit() = default;
};

TEST_F(Unit, yolodecorator) {
  FLAGS_minloglevel = 0;
  char *in = nullptr;
  const uint32_t kInputSize = 333396;
  in = ReadFromFile("/home/yankai.yan/workbase/codeLib/refactor/tests/bin/torch_out.bins", kInputSize * sizeof(float));
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
  free(in);
  LOG(INFO) << "Run SUCCESS!";
}

TEST_F(Unit, parseconfig) {
  uint32_t kInputUnit = 1 * 3 * 384 * 672;
  cv::Mat img = cv::imread("/home/yankai.yan/workbase/codeLib/refactor/tests/bin/input.jpg");
  ASSERT_NE(img.empty(), true);
  std::string model_cfg_file = "/home/yankai.yan/workbase/codeLib/refactor/modules/models.cfg";
  std::vector<pipeline::ModelCfgPtr> cfg_vec;
  int ret = ParseConfig::ParseConfigFromProto(model_cfg_file, &cfg_vec);
  ASSERT_EQ(0, ret);
}