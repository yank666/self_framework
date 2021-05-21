//
// Created by yankai on 2021/1/28.
//
#include "common_test.h"
#include <fstream>
#include <thread>
#include "gtest/gtest.h"
#include "glog/logging.h"
#include "src/parse/parse_config.h"
#include "src/parse/parse_video.h"
#include "src/pipeline/pipeline.h"
#include "opencv2/opencv.hpp"

using namespace pipeline;

class BenchMark : public CommonTest {
 public:
  BenchMark() = default;
  ~BenchMark() = default;
};

TEST_F(BenchMark, st) {
  uint32_t kInputUnit = 1 * 3 * 384 * 672;
  FLAGS_minloglevel = 0;
  std::string model_cfg_file = "/home/yankai.yan/workbase/codeLib/refactor/modules/config_file/models.cfg";
  std::vector<pipeline::ModelCfgPtr> cfg_vec;
  int ret = ParseConfig::ParseConfigFromProto(model_cfg_file, &cfg_vec);
  ASSERT_EQ(0, ret);
  cv::Mat img = cv::imread("./input.jpg", 1);
  ASSERT_NE(img.empty(),false);
  std::vector<uint32_t> input_size;
  input_size.push_back(kInputUnit * sizeof(uint8_t));
  pipeline::Pipeline ss;

  ss.InitPipeline(cfg_vec);
  ss.PushDatatoPipeline((char **)(&img.data), input_size, img.cols,  img.rows);
  ss.RunPipeline();
  LOG(INFO) << "Run SUCCESS!";
}

TEST_F(BenchMark, readnull) {
  cv::Mat img = cv::imread("/home/yankai.yan/workbase/codeLib/refactor/tests/bin/input.jpg");
//  ASSERT_EQ(img.empty(), true);
  std::string model_cfg_file = "/home/yankai.yan/workbase/codeLib/refactor/modules//models.cfg";
  std::vector<pipeline::ModelCfgPtr> cfg_vec;
  int ret = ParseConfig::ParseConfigFromProto(model_cfg_file, &cfg_vec);
  ASSERT_EQ(0, ret);
  auto pipeline_ptr = std::make_shared<Pipeline>();
  ASSERT_NE(nullptr, pipeline_ptr);
  std::vector<uint32_t> input_size;
  input_size.push_back(3 * 384 * 672 * sizeof(uint8_t));
  pipeline_ptr->InitPipeline(cfg_vec);
  pipeline_ptr->PushDatatoPipeline((char **)&img.data, input_size, 672, 384);
  pipeline_ptr->RunPipeline();
  LOG(INFO) << "Run SUCCESS!";
}

TEST_F(BenchMark, readvideo) {
  int ret = 0;
  std::shared_ptr<ParseVideo> stream_ptr = std::make_shared<ParseVideo>();
  ASSERT_NE(stream_ptr, nullptr);
  std::thread thread_stream(std::bind(&ParseVideo::ParseVideoFromStream, stream_ptr, "./video.mp4", 0));
  ASSERT_EQ(ret, 0);
  std::string model_cfg_file = "./models.cfg";
  std::vector<pipeline::ModelCfgPtr> cfg_vec;
  ret = ParseConfig::ParseConfigFromProto(model_cfg_file, &cfg_vec);
  ASSERT_EQ(0, ret);
  auto pipeline_ptr = std::make_shared<Pipeline>();
  ASSERT_NE(nullptr, pipeline_ptr);
  std::vector<uint32_t> input_size;
  input_size.push_back(0 * sizeof(uint8_t));
  pipeline_ptr->InitPipeline(cfg_vec);
  while(!stream_ptr->GetReady()) {
    sleep(1);
  }
  int count_run_turn  = 0;
  int total_turn = stream_ptr->GetTotalFrames();
  int break_flag = 0;
  while(count_run_turn < total_turn && stream_ptr->GetReady()) {
    cv::Mat img;
    if (!stream_ptr->GetParseDate(img)) {
      continue;
    }

    pipeline_ptr->PushDatatoPipeline((char **)&(img.data), input_size, 0, 0);
    pipeline_ptr->RunPipeline();
    DLOG(INFO) << "BREAK" << stream_ptr->GetVideoFrames() << " " << count_run_turn;

    count_run_turn++;
  }

  thread_stream.join();
  LOG(INFO) << "Run SUCCESS!, run total turn: " << count_run_turn;
}
