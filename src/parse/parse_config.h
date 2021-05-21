//
// Created by yankai.yan on 2021/3/11.
//

#ifndef SELF_ARCHITECTURE_PARSECONFIG_H
#define SELF_ARCHITECTURE_PARSECONFIG_H
#include <string>
#include <vector>
#include <memory>
namespace pipeline {

struct ModelConfig {
  uint32_t model_position_;
  std::string model_name_;
  std::string model_type_;
  bool is_inferengine_;
  std::string model_binary_;
  std::string data_format_;
  std::vector<std::vector<uint32_t>> model_inshape_;
  std::vector<std::vector<uint32_t>> model_outshape_;
  std::vector<float> model_norm_;
  std::vector<float> model_mean_;
};

using ModelCfgPtr = std::shared_ptr<ModelConfig>;
using ModelCfgList = std::vector<std::shared_ptr<ModelConfig>>;

class ParseConfig {
 public:
  ParseConfig() = default;
  ~ParseConfig() = default;
  static int ParseConfigFromProto(const std::string &cfg_file,
                                  pipeline::ModelCfgList *model_list);
};
}  // namespace pipeline
#endif  // SELF_ARCHITECTURE_PARSECONFIG_H
