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
  std::string model_cfg_file = "models.cfg";
  std::vector<pipeline::ModelCfgPtr> cfg_vec;
  int ret = ParseConfig::ParseConfigFromProto(model_cfg_file, &cfg_vec);
  ASSERT_EQ(0, ret);
  cv::Mat img = cv::imread("./input.jpg", 1);
  ASSERT_EQ(img.empty(), false);
  std::vector<uint32_t> input_size;
  input_size.push_back(kInputUnit * sizeof(uint8_t));
  pipeline::Pipeline ss;

  ss.InitPipeline(cfg_vec);

  for (int i = 0; i < 1; i++) {
    ss.PushDatatoPipeline((char **)(&img.data), input_size, img.cols, img.rows);
    ss.RunPipeline();
  }
  LOG(INFO) << "Run SUCCESS!";
}

TEST_F(BenchMark, init) {
  std::string model_cfg_file = "models.cfg";
  std::vector<pipeline::ModelCfgPtr> cfg_vec;
  int ret = ParseConfig::ParseConfigFromProto(model_cfg_file, &cfg_vec);
  ASSERT_EQ(0, ret);
  auto pipeline_ptr = std::make_shared<Pipeline>();
  ASSERT_NE(nullptr, pipeline_ptr);
  pipeline_ptr->InitPipeline(cfg_vec);
  LOG(INFO) << "Run SUCCESS!";
}

TEST_F(BenchMark, readvideo) {
  int ret = 0;
  std::shared_ptr<ParseVideo> stream_ptr = std::make_shared<ParseVideo>();
  ASSERT_NE(stream_ptr, nullptr);
  std::thread thread_stream(
    std::bind(&ParseVideo::ParseVideoFromStream, stream_ptr, "./video.mp4", 0));
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
  while (!stream_ptr->GetReady()) {
    sleep(1);
  }
  int count_run_turn = 0;
  int total_turn = stream_ptr->GetTotalFrames();
  LOG(INFO) << "SUCCESS at get ready";
  int break_flag = 0;
  while (count_run_turn < total_turn && stream_ptr->GetReady()) {
    cv::Mat img;
    if (!stream_ptr->GetParseDate(img)) {
      continue;
    }

    pipeline_ptr->PushDatatoPipeline((char **)&(img.data), input_size, 0, 0);
    pipeline_ptr->RunPipeline();
    DLOG(INFO) << "BREAK" << stream_ptr->GetVideoFrames() << " "
               << count_run_turn;

    count_run_turn++;
  }

  thread_stream.join();
  LOG(INFO) << "Run SUCCESS!, run total turn: " << count_run_turn;
}

TEST_F(BenchMark, video) {
  LOG(INFO) << "Begin BenchMark.video test";
  int ret = 0;
  std::shared_ptr<ParseVideo> stream_ptr = std::make_shared<ParseVideo>();
  ASSERT_NE(stream_ptr, nullptr);
  std::thread thread_stream(std::bind(
    &ParseVideo::ParseVideoFromStream, stream_ptr,
    "/home/yankai.yan/workbase/codeLib/refactor/tests/bin/video.avi", 0));
  ASSERT_EQ(ret, 0);
  while (!stream_ptr->GetReady()) {
    sleep(1);
  }
  int count_run_turn = 0;
  LOG(INFO) << "Begin BenchMark.video test 1";
  int total_turn = stream_ptr->GetTotalFrames();
  LOG(INFO) << "SUCCESS at get ready";
  int break_flag = 0;
  while (count_run_turn < total_turn && stream_ptr->GetReady()) {
    cv::Mat img;
    if (!stream_ptr->GetParseDate(img)) {
      continue;
    }
    if (count_run_turn % 1 == 0) {
      cv::imwrite("./out/" + std::to_string(count_run_turn) + ".jpg", img);
    }
    DLOG(INFO) << "BREAK" << stream_ptr->GetVideoFrames() << " "
               << count_run_turn;
    count_run_turn++;
  }

  thread_stream.join();
  LOG(INFO) << "Run SUCCESS!, run total turn: " << count_run_turn;
}

TEST_F(BenchMark, multiimg) {
  std::vector<std::string> name_vec;
  GetFileNames("./out", "jpg", &name_vec);
  LOG(INFO) << "Total image size is: " << name_vec.size();

  const uint32_t kInputSize = 1280 * 720 * 3;
  std::vector<uint32_t> input_size;
  input_size.push_back(kInputSize * sizeof(uint8_t));

  std::string model_cfg_file = "./models.cfg";
  std::vector<pipeline::ModelCfgPtr> cfg_vec;
  int ret = ParseConfig::ParseConfigFromProto(model_cfg_file, &cfg_vec);
  ASSERT_EQ(0, ret);
  auto pipeline_ptr = std::make_shared<Pipeline>();
  ASSERT_NE(nullptr, pipeline_ptr);
  pipeline_ptr->InitPipeline(cfg_vec);
  uint32_t turns = 0;
  cv::Mat out;
  for (auto name : name_vec) {
    cv::Mat img = cv::imread(name);
    cv::resize(img, out, cv::Size(672, 384));
    LOG(INFO) << "=================" << name << "========================";
    pipeline_ptr->PushDatatoPipeline((char **)&(out.data), input_size, out.cols,
                                     out.rows);
    pipeline_ptr->RunPipeline();
    LOG(INFO) << "================="
              << " Complete turn :" << turns << "========================";
    turns++;
  }
  LOG(INFO) << "Run SUCCESS!";
}

TEST_F(BenchMark, ffmpeg) {
  LOG(INFO) << "Begin BenchMark.ffmpeg test";
  int ret = 0;
  std::shared_ptr<ParseVideo> stream_ptr = std::make_shared<ParseVideo>();
  ASSERT_NE(stream_ptr, nullptr);
  const std::string file_path = "video.avi";
  std::thread thread_stream(
    std::bind(&ParseVideo::ParseVideoByFFmpeg, stream_ptr, file_path));
  ASSERT_EQ(ret, 0);
  while (!stream_ptr->GetReady()) {
    sleep(1);
    if (stream_ptr->stop_) {
      LOG(INFO) << "Parse by ffmpeg fail, parse video quit early!";
      thread_stream.join();
      ASSERT_EQ(1, 0);
    }
  }
  int count_run_turn = 0;
  LOG(INFO) << "SUCCESS at get ready";
  int break_flag = 0;
  while (!stream_ptr->stop_ && stream_ptr->GetReady()) {
    cv::Mat img;
    if (!stream_ptr->GetParseDate(img)) {
      continue;
    }
    cv::imwrite("./out/" + std::to_string(count_run_turn) + ".jpg", img);
    DLOG(INFO) << "BREAK" << stream_ptr->GetVideoFrames() << " "
               << count_run_turn;
    count_run_turn++;
  }

  thread_stream.join();
  LOG(INFO) << "Run SUCCESS!, run total turn: " << count_run_turn;
}