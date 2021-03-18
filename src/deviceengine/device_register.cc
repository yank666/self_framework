//
// Created by yankai.yan on 2021/3/17.
//
#include "device_register.h"

namespace  device {
const std::unordered_map<std::string, uint32_t> kSwitchModelType = {
    {"trt", 0},
    {"nb", 1},
};
void DeviceInferenceFactory::Register(const std::string &name_inference,
    const std::string &type_inference, DeviceInferPtr device_inferencer) {
  uint32_t model_type  = kSwitchModelType.at(type_inference);
  switch (model_type) {
  case 0 : {
    trt_creator_.insert(std::make_pair(name_inference, device_inferencer));
  }
  case 1 : {
    nb_creator_.insert(std::make_pair(name_inference, device_inferencer));
  }
  default:
    LOG(ERROR) << "Can't support model type in device Register!";
    return;
  }
}
DeviceInferenceFactory& DeviceInferenceFactory::GetInstance() {
  static DeviceInferenceFactory instance;
  return instance;
}

DeviceInferPtr DeviceInferenceFactory::GetDeviceInference(
                               const std::string name, const std::string type) {
  DeviceInferPtr ret_device_infer = nullptr;
  switch (kSwitchModelType.at(type)) {
  case 0 : {
    ret_device_infer = SelectEnginebyType(name, &trt_creator_);
  }
  case 1 : {
    ret_device_infer = SelectEnginebyType(name, &nb_creator_);
  }
  default:
    LOG(ERROR) << "Can't support model type in device Register!";
    return nullptr;
  }
}

DeviceInferPtr SelectEnginebyType(const std::string name,
std::unordered_map<std::string, DeviceInferPtr> *engine_map) {
  auto iter = engine_map->find(name);
  if (iter == engine_map->end()) {
    LOG(ERROR) << "DeviceInferenceFactory can't find inference_creator_ KEY: " << iter->first;
    return nullptr;
  }
  return iter->second;
}
}