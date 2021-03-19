#include <iostream>
#include <vector>
#include <fstream>
#include <set>
#include <iterator>
//#include <omp.h>
#include "glog/logging.h"
//#include "Eigen/Dense"
#include "src/pipeline/pipeline.h"
#include "src/pipeline/register_stage.h"
#include "src/pipeline/face_decorator_stage.h"
#include "src/parse/parse_config.h"

char* ReadFromFile(std::string file_name, size_t memlen) {
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
  return buf;
};

class Bad : public std::enable_shared_from_this<Bad>
{
public:
  Bad() = default;
  std::shared_ptr<Bad> getptr() {
    return  shared_from_this();
  }
  ~Bad() { std::cout << "Bad::~Bad() called" << std::endl; }
};

int main(int argc, char **argv) {
  FLAGS_minloglevel = 0;
  std::ios_base::sync_with_stdio;
  google::InitGoogleLogging(argv[0]);

  std::string model_cfg_file = "/home/yankai.yan/workbase/codeLib/refactor/modules/models.cfg";
  std::vector<pipeline::ModelCfgPtr> cfg_vec;
  pipeline::ParseConfig::ParseConfigFromProto(model_cfg_file, &cfg_vec);

  pipeline::Pipeline ss;
  char **input= nullptr;
  
  ss.InitPipeline(cfg_vec, input);
  ss.RunPipeline();
  return 0;

}
