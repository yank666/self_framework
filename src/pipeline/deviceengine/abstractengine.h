//
// Created by yankai.yan on 2021/3/18.
//

#ifndef SELF_ARCHITECTURE_ABSTRACTENGINE_H
#define SELF_ARCHITECTURE_ABSTRACTENGINE_H
#include "src/parse/parse_config.h"
#include <string>
namespace device {
using ModelCfgPtr = pipeline::ModelCfgPtr;

class Context {
public:
  Context() = default;
  ~Context() = default;

  uint32_t channel_;
  std::string name_;
  std::vector<uint32_t> datasize_;
  std::vector<std::vector<float>> dataflow_;
  std::vector<std::vector<float>> out_dataflow_;
  std::vector<std::vector<uint32_t>> out_shape_;
};
using contextPtr = std::shared_ptr<Context>;

class AbstractEngine {
public:
  AbstractEngine(ModelCfgPtr model_cfg_ptr)
      : model_cfg_(model_cfg_ptr){};
  virtual ~AbstractEngine() = default;
  virtual int CreateGraph(const contextPtr& cur_context_ptr) = 0;
  virtual int RunProcess(const contextPtr& cur_context_ptr) = 0;
  bool SetModelCfg(ModelCfgPtr model_cfg_ptr);
protected:
  pipeline::ModelCfgPtr model_cfg_;
};
}
#endif // SELF_ARCHITECTURE_ABSTRACTENGINE_H
