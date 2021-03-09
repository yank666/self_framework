//
// Created by yankai on 2021/3/5.
//

#ifndef SELF_ARCHITECTURE_PIPELINE_H
#define SELF_ARCHITECTURE_PIPELINE_H

#include <vector>
#include <string>
#include <memory>
namespace pipeline {
class Context {
public:
  Context() = default;
  ~Context() = default;
  
  uint32_t channel_;
  std::string name_;
  std::vector<uint32_t> datasize_;
  std::vector<std::vector<float>> dataflow_;
  std::vector<std::vector<float>> dataflow_temp_;
};
using contextPtr = std::shared_ptr<Context>;

struct ModelConfig {
  uint32_t model_position_;
  std::string model_name_;
  std::string model_binary_;
  std::vector<std::vector<uint32_t>> model_inshape_;
  std::vector<std::vector<uint32_t>> model_outshape_;
  std::vector<uint32_t> model_norm_;
  std::vector<uint32_t> model_mean_;
};
using ModelCfgPtr = std::shared_ptr<ModelConfig>;

class AbstractStage {
public:
  AbstractStage(const ModelCfgPtr& model_cfg) :stage_context_{nullptr},
    stage_cfg_(model_cfg){
    if (model_cfg != nullptr) {
      stage_name_ = model_cfg->model_name_;
    }
  };
  ~AbstractStage() = default;
  virtual bool RunStage(char *input_array) = 0;
  bool CreateContextFromCfg();
  const std::string GetModelName(){return stage_name_;}
  contextPtr GetContext() {return stage_context_;};
  ModelCfgPtr GetModelCfg() {return stage_cfg_;};
protected:
  std::string stage_name_;
  contextPtr stage_context_;
  ModelCfgPtr stage_cfg_;
};
using AbstractStagePtr = std::shared_ptr<AbstractStage>;

class DeviceStage : public AbstractStage {
public:
  DeviceStage(const ModelCfgPtr& model_cfg) : AbstractStage(model_cfg){};
  ~DeviceStage() = default;
  bool RunStage(char *input_array);
};
using DeviceStagePtr = std::shared_ptr<DeviceStage>;

class DecoratorStage : public AbstractStage {
public:
  DecoratorStage() : extra_stage_ptr_(nullptr), AbstractStage(nullptr){};
  ~DecoratorStage() = default;
  bool RunStage(char *input_array);
  bool AddProcess(const DeviceStagePtr &device_ptr);
protected:
  virtual bool StagePreProcess(char *input_array);
  virtual bool StagePostProcess();
  bool RunSubStage(char *input_array) {return extra_stage_ptr_->RunStage(input_array);};
  DeviceStagePtr extra_stage_ptr_;
};
using DecorStagePtr = std::shared_ptr<DecoratorStage>;

class Pipeline {
public:
  Pipeline() = default;
  ~Pipeline() = default;
  bool InitPipeline(const std::vector<ModelCfgPtr> & model_cfgs,
                        char **input_data);
  void SetStage(const AbstractStagePtr &stage_ptr);
  const DeviceStagePtr GetStage(const uint32_t &poisiton);
  const DeviceStagePtr FindStage(std::string stage_name);
  void RunPipeline();
  void GetRunResult();
  void RegisterProcess(const ModelCfgPtr &model_cfgs);
  bool SetDataFlowBuf(const std::vector<AbstractStagePtr> &cur_stage_vec);
protected:
  std::vector<std::vector<AbstractStagePtr>> stages_;
};
}
#endif //SELF_ARCHITECTURE_PIPELINE_H
