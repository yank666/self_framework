//
// Created by yankai on 2021/1/28.
//
#include "gtest/gtest.h"
#include <fstream>
#include "glog/logging.h"
#include "src/parse/parse_config.h"
#include "src/pipeline/pipeline.h"

class BenchMark : public testing::Test{
public:
  BenchMark() = default;
  ~BenchMark()= default;
  static void SetUpTestCase(){};
  static void TearDownTestCase(){};
  char* ReadFromFile(std::string file_name, size_t memlen);
  // every TEST_F macro will enter one
  virtual void SetUp(){};
  virtual void TearDown(){};
};

char* BenchMark::ReadFromFile(std::string file_name, size_t memlen) {
  std::ifstream fin(file_name, std::ios::binary);
  if (!fin.good()) {
    LOG(ERROR) << "file: " << file_name << " is not exist";
    return nullptr;
  }
  
  if (!fin.is_open()) {
    LOG(ERROR) << "Cannot open label file " << file_name;
    return nullptr;
  }
  fin.seekg (0, fin.end);
  size_t buflen  = fin.tellg();
  if (buflen != memlen) {
    LOG(ERROR) << "Read file " << file_name << "failed, memory length is not excepted";
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
  FLAGS_minloglevel = 0;
  std::string model_cfg_file = "./models.cfg";
  std::vector<pipeline::ModelCfgPtr> cfg_vec;
  pipeline::ParseConfig::ParseConfigFromProto(model_cfg_file, &cfg_vec);

  pipeline::Pipeline ss;
  char **input= nullptr;
  ss.InitPipeline(cfg_vec, input);
  ss.RunPipeline();
  LOG(INFO) << "Run SUCCESS!";
}
TEST_F(BenchMark, Accuracy_test) {

}
