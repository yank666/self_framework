//
// Created by yankai on 2021/1/28.
//
#include "gtest/gtest.h"
#include <fstream>
#include "glog/logging.h"
#include "src/parse/parse_config.h"
#include "src/pipeline/pipeline.h"

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
  uint8_t *print_ptr = reinterpret_cast<uint8_t *>(in);
  LOG(INFO) << (int32_t)(*print_ptr) << " " << (int32_t)*(print_ptr+ 1) << " "<< (int32_t)*(print_ptr+ 2);
  std::vector<uint32_t> input_size;
  input_size.push_back(kInputUnit * sizeof(uint8_t));
  pipeline::Pipeline ss;
//  char* input_array[] = {in};
  ss.InitPipeline(cfg_vec, &in, input_size);
  ss.RunPipeline();
  free(in);
  LOG(INFO) << "Run SUCCESS!";
}

TEST_F(BenchMark, time) {

}
