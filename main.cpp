#include <iostream>
#include <vector>
#include <fstream>
#include <set>
#include <iterator>
//#include <omp.h>
#include <glog/logging.h>
//#include "Eigen/Dense"
#include "src/pipeline/pipeline.h"
#include "src/pipeline/register_stage.h"
#include "src/pipeline/face_decorator_stage.h"

struct DereferenceLess {
  template <typename PtrType>
  bool operator()(PtrType pT1, // 参数是值传递的，
                  PtrType pT2) const // 因为我们希望它们
  { // 是（或行为像）指针
    return *pT1 > *pT2;
  }
};

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
}

int main(int argc, char **argv) {
  FLAGS_minloglevel = 0;
  std::ios_base::sync_with_stdio;
  google::InitGoogleLogging(argv[0]);
  pipeline::ModelCfgPtr modelcfg = std::make_shared<pipeline::ModelConfig>();
  modelcfg->model_position_ = 0;
  modelcfg->model_name_= "fcos";
  modelcfg->model_binary_ = "D:\\document\\teamShare\\TeamFile\\DDK_6.4.2"
                            ".1-doc\\DDK_6.4.2.1";
  std::vector<uint32_t > inshape = {1,3,64,64};
  std::vector<std::vector<uint32_t >> in_vec;
  in_vec.push_back(inshape);
  modelcfg->model_inshape_ = in_vec;
  modelcfg->model_outshape_ = in_vec;
  
  pipeline::ModelCfgPtr modelcfg1 = std::make_shared<pipeline::ModelConfig>();
  modelcfg1->model_position_ = 1;
  modelcfg1->model_name_= "face";
  modelcfg1->model_binary_ = "D:\\document\\teamShare\\TeamFile\\DDK_6.4.2"
                            ".1-doc\\DDK_6.4.2.1";
  std::vector<uint32_t > inshape1 = {1,3,128,128};
  std::vector<std::vector<uint32_t >> in_vec1;
  in_vec1.push_back(inshape1);
  modelcfg1->model_inshape_ = in_vec1;
  modelcfg1->model_outshape_ = in_vec1;
  pipeline::DeviceInferenceRegister g_face("face",
      std::make_shared<pipeline::FaceDerocatestage>());
  std::vector<pipeline::ModelCfgPtr> cfg_vec;
  cfg_vec.push_back(modelcfg);
  cfg_vec.push_back(modelcfg1);
  pipeline::Pipeline ss;
  char **input= nullptr;
  
  ss.InitPipeline(cfg_vec, input);
  ss.RunPipeline();
  return 0;

}
