//
// Created by yankai on 2021/3/5.
//

#ifndef SELF_ARCHITECTURE_PIPELINE_H
#define SELF_ARCHITECTURE_PIPELINE_H

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "glog/logging.h"
#include "../parse/parse_config.h"
#include "../deviceengine/abstractengine.h"
#include "../deviceengine/amlogicengine/amlogical_engine_infer.h"


namespace pipeline {
class AbstractStage;
using AbstractStagePtr = std::shared_ptr<AbstractStage>;
class Context;
using contextPtr = std::shared_ptr<Context>;
using ProcessContextMap = std::unordered_map<std::string,
                        std::pair<AbstractStagePtr, contextPtr>>;
using ProcessContext = std::pair<AbstractStagePtr, contextPtr>;

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

class AbstractStage {
public:
  AbstractStage(const ModelCfgPtr& model_cfg) : stage_cfg_(model_cfg){
    if (model_cfg != nullptr) {
      stage_name_ = model_cfg->model_name_;
    }
  };
  ~AbstractStage() = default;
  virtual bool RunStage(const ProcessContextMap &conext_map) = 0;
  bool CreateContextFromCfg();
  const std::string GetModelName(){return stage_name_;}
  ModelCfgPtr GetModelCfg() {return stage_cfg_;};
protected:
  std::string stage_name_;
  ModelCfgPtr stage_cfg_;
};

class DeviceStage : public AbstractStage {
public:
  DeviceStage(const ModelCfgPtr& model_cfg) : AbstractStage(model_cfg){
    switch (1) {
      case 0 : {

        break;
      }
      case 1 : {
        engine_ = std::make_shared<device::AmlogicEngine>(model_cfg);
        break;
      }
      default:
        LOG(ERROR) << "Init DeviceStaeg fail, model type has not support";
    }
  };
  ~DeviceStage() = default;
  bool FillStagebyEngine();
  bool RunStage(const ProcessContextMap &conext_map);
private:
  std::shared_ptr<device::AbstractEngine> engine_;
};
using DeviceStagePtr = std::shared_ptr<DeviceStage>;

class DecoratorStage : public AbstractStage {
public:
  DecoratorStage() : extra_stage_ptr_(nullptr), AbstractStage(nullptr){};
  ~DecoratorStage() = default;
  bool RunStage(const ProcessContextMap &conext_map);
  bool AddProcess(const DeviceStagePtr &device_ptr);
protected:
  virtual bool StagePreProcess(const ProcessContextMap &conext_map) = 0;
  virtual bool StagePostProcess(const ProcessContextMap &conext_map) = 0;
  bool RunSubStage(const ProcessContextMap &conext_map) {
                            return extra_stage_ptr_->RunStage(conext_map);}
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
  void RegisterStage(const ModelCfgPtr &model_cfg);
  bool SetDataFlowBuf(const std::vector<AbstractStagePtr> &cur_stage_vec);
  bool CreateContexts(const contextPtr &context_ptr, const ModelCfgPtr &model_cfg);
  contextPtr GetStageContextbyName(const std::string &stage_name);
protected:
  std::vector<std::vector<AbstractStagePtr>> stages_;
  ProcessContextMap process_context_;
};
}
#endif //SELF_ARCHITECTURE_PIPELINE_H
