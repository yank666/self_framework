//
// Created by yankai on 2021/1/28.
//
#include "gtest/gtest.h"
#include <fstream>
#include <thread>
#include "glog/logging.h"
#include "src/parse/parse_config.h"
#include "src/parse/parse_video.h"
#include "src/pipeline/pipeline.h"
#include "opencv2/opencv.hpp"

using namespace pipeline;

class BenchMark : public testing::Test {
 public:
  BenchMark() = default;
  ~BenchMark() = default;
  static void SetUpTestCase(){};
  static void TearDownTestCase(){};
  char *ReadFromFile(std::string file_name, size_t memlen);
  // every TEST_F macro will enter one
  virtual void SetUp(){};
  virtual void TearDown(){};
};

char *BenchMark::ReadFromFile(std::string file_name, size_t memlen) {
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


TEST_F(BenchMark, st) {
  uint32_t kInputUnit = 1 * 3 * 384 * 672;
  FLAGS_minloglevel = 0;
  std::string model_cfg_file = "/home/yankai.yan/workbase/codeLib/refactor/modules/models.cfg";
  std::vector<pipeline::ModelCfgPtr> cfg_vec;
  int ret = ParseConfig::ParseConfigFromProto(model_cfg_file, &cfg_vec);
  ASSERT_EQ(0, ret);
  char *in = nullptr;
  in = ReadFromFile("/home/yankai.yan/workbase/codeLib/refactor/bin/input_ori.bin", kInputUnit * sizeof(uint8_t));
  ASSERT_NE(nullptr, in);
  std::vector<uint32_t> input_size;
  input_size.push_back(kInputUnit * sizeof(uint8_t));
  pipeline::Pipeline ss;
//  char* input_array[] = {in};
  ss.InitPipeline(cfg_vec);
  ss.PushDatatoPipeline(&in, input_size);
  ss.RunPipeline();
  free(in);
  LOG(INFO) << "Run SUCCESS!";
}

TEST_F(BenchMark, readnull) {
  cv::Mat img = cv::imread("input.jpg");
  ASSERT_EQ(img.empty(), true);
  std::string model_cfg_file = "/home/yankai.yan/workbase/codeLib/refactor/modules/models.cfg";
  std::vector<pipeline::ModelCfgPtr> cfg_vec;
  int ret = ParseConfig::ParseConfigFromProto(model_cfg_file, &cfg_vec);
  ASSERT_EQ(0, ret);
  auto pipeline_ptr = std::make_shared<Pipeline>();
  ASSERT_NE(nullptr, pipeline_ptr);
  std::vector<uint32_t> input_size;
  input_size.push_back(0 * sizeof(uint8_t));
  pipeline_ptr->InitPipeline(cfg_vec);
  pipeline_ptr->PushDatatoPipeline((char **)&img.data, input_size);
  pipeline_ptr->RunPipeline();
  LOG(INFO) << "Run SUCCESS!";
}

TEST_F(BenchMark, readvideo) {
  int ret = 0;
  std::shared_ptr<ParseVideo> stream_ptr = std::make_shared<ParseVideo>();
  ASSERT_NE(stream_ptr, nullptr);
  std::thread thread_stream(std::bind(&ParseVideo::ParseVideoFromStream, stream_ptr, "/home/yankai.yan/workbase/codeLib/refactor/tests/bin/video.mp4", 0));
  ASSERT_EQ(ret, 0);
  std::string model_cfg_file = "/home/yankai.yan/workbase/codeLib/refactor/modules/models.cfg";
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
  while(count_run_turn < 200) {
    cv::Mat img;
    if (!stream_ptr->GetParseDate(img)) {
      continue;
    }
    pipeline_ptr->PushDatatoPipeline((char **)&(img.data), input_size);
    pipeline_ptr->RunPipeline();
    LOG(INFO) << "BREAK" << stream_ptr->GetVideoFrames() << " " << count_run_turn;

    count_run_turn++;
  }

  thread_stream.join();
  LOG(INFO) << "Run SUCCESS!, run total turn: " << count_run_turn;
}
