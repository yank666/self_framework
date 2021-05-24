//
// Created by yankai on 2021/3/5.
//
#ifndef SELF_ARCHITECTURE_STAGE_REGISTER_H
#define SELF_ARCHITECTURE_STAGE_REGISTER_H
#include "pipeline.h"
#include <unordered_map>
namespace pipeline {
class DeviceInferenceFactory {
 public:
  ~DeviceInferenceFactory() = default;

  void Register(const std::string &name_inference,
                std::shared_ptr<DecoratorStage> device_inferencer) {
    inference_creator_[name_inference] = device_inferencer;
  }

  static DeviceInferenceFactory &GetInstance() {
    static DeviceInferenceFactory instance;
    return instance;
  }

  std::shared_ptr<DecoratorStage> GetDeviceInference(std::string name) {
    auto iter = inference_creator_.find(name);
    if (iter == inference_creator_.end()) {
      LOG(ERROR) << "DeviceInferenceFactory can't find " << iter->first;
      return nullptr;
    }
    return iter->second;
  }

 private:
  DeviceInferenceFactory() = default;

  std::unordered_map<std::string, std::shared_ptr<DecoratorStage>>
    inference_creator_;
};

class DeviceInferenceRegister {
 public:
  DeviceInferenceRegister(const std::string &name_inference,
                          std::shared_ptr<DecoratorStage> device_inferencer) {
    DeviceInferenceFactory::GetInstance().Register(name_inference,
                                                   device_inferencer);
  }

  ~DeviceInferenceRegister() = default;
};

#define REG_Stage(name, decoratorstage) \
  DeviceInferenceRegister g_##name##_Inference_Reg(#name, decoratorstage);
}  // namespace pipeline
#endif  // SELF_ARCHITECTURE_STAGE_REGISTER_H
