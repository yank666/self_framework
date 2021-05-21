//
// Created by yankai on 2021/3/5.
//

#ifndef SELF_ARCHITECTURE_PIPELINE_H
#define SELF_ARCHITECTURE_PIPELINE_H

#include "deviceengine/abstractengine.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <atomic>
#include "src/parse/parse_config.h"
#include "glog/logging.h"
#include "deviceengine/trtengine/trt_engine.h"

namespace pipeline {
using device::contextPtr;
class AbstractStage;
using AbstractStagePtr = std::shared_ptr<AbstractStage>;

using ProcessContext = std::pair<AbstractStagePtr, contextPtr>;
using ProcessContextMap = std::unordered_map<std::string, ProcessContext>;


class AbstractStage {
 public:
  AbstractStage(const ModelCfgPtr &model_cfg) : stage_cfg_(model_cfg) {
    if (model_cfg != nullptr) {
      stage_name_ = model_cfg->model_name_;
    }
  };
  ~AbstractStage() = default;
  virtual bool RunStage(const contextPtr &conext_ptr,
                        const ProcessContextMap &contextMap) = 0;
  bool CreateContextFromCfg();
  std::string GetModelName() { return stage_name_; }
  ModelCfgPtr GetModelCfg() { return stage_cfg_; };
  void ConnectTailStage(std::string stage_name) {
    tail_stages.push_back(stage_name);
  };
  const std::vector<std::string> &GetTailStage() { return tail_stages; };

 protected:
  std::string stage_name_;
  std::vector<std::string> tail_stages;
  ModelCfgPtr stage_cfg_;
};

class DeviceStage : public AbstractStage {
 public:
  DeviceStage(const ModelCfgPtr &model_cfg);
  ~DeviceStage() = default;
  bool RunStage(const contextPtr &conext_ptr,
                const ProcessContextMap &contextMap);
  bool FillStagebyEngine(const contextPtr &conext_ptr);

 private:
  std::shared_ptr<device::AbstractEngine> engine_;
};
using DeviceStagePtr = std::shared_ptr<DeviceStage>;

class DecoratorStage : public AbstractStage {
 public:
  DecoratorStage() : extra_stage_ptr_(nullptr), AbstractStage(nullptr){};
  ~DecoratorStage() = default;
  bool RunStage(const contextPtr &conext_ptr,
                const ProcessContextMap &contextMap);
  bool AddProcess(const DeviceStagePtr &device_ptr);

 protected:
  virtual bool StagePreProcess(const contextPtr &conext_ptr,
                               const ProcessContextMap &contextMap) = 0;
  virtual bool StagePostProcess(const contextPtr &conext_ptr,
                                const ProcessContextMap &contextMap) = 0;
  bool RunSubStage(const contextPtr &conext_ptr,
                   const ProcessContextMap &contextMap) {
    if (extra_stage_ptr_ == nullptr) {
      return true;
    }
    return extra_stage_ptr_->RunStage(conext_ptr, contextMap);
  }
  DeviceStagePtr extra_stage_ptr_;
};
using DecorStagePtr = std::shared_ptr<DecoratorStage>;

class Pipeline {
 public:
  Pipeline() = default;
  ~Pipeline() = default;
  bool InitPipeline(const std::vector<ModelCfgPtr> &model_cfgs);
  void RunPipeline();
  bool PushDatatoPipeline(char **input_data,
                          const std::vector<uint32_t> &input_size,
                          const uint32_t &input_width,
                          const uint32_t &input_height);
  contextPtr GetStageContextbyName(const std::string &stage_name);
  bool GetStatusofPipeline() { return is_ready_.load(); }
  void DeliveryContext(const AbstractStagePtr &cur_staga,
                       const contextPtr &cur_context);

 protected:
  void RegisterStage(const ModelCfgPtr &model_cfg);
  void RegisterEngine(const ModelCfgPtr &model_cfg);
  bool CreateContexts(const contextPtr &context_ptr,
                      const ModelCfgPtr &model_cfg);

  void SetStage(const AbstractStagePtr &stage_ptr);
  const DeviceStagePtr GetStage(const uint32_t &poisiton);
  const DeviceStagePtr FindStage(const std::string &stage_name);

  std::atomic<bool> is_ready_{false};
  std::vector<std::vector<AbstractStagePtr>> stages_;
  ProcessContextMap process_context_;
};
}  // namespace pipeline
#endif  // SELF_ARCHITECTURE_PIPELINE_H
