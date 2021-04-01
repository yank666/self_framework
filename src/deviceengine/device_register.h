//
// Created by yankai.yan on 2021/3/17.
//

#ifndef SELF_ARCHITECTURE_DEVICE_REGISTER_H
#define SELF_ARCHITECTURE_DEVICE_REGISTER_H
#include <string>
#include <unordered_map>
#include <memory>
//#include "glog/logging.h"
#include "abstractengine.h"
//#include "vsi_nn_pub.h"
namespace device {
using AbsEnginePtr = std::shared_ptr<AbstractEngine>;
class DeviceInferenceFactory {
public:
  DeviceInferenceFactory() = default;
  ~DeviceInferenceFactory() = default;
  void Register(const std::string &name_inference,
            const std::string &type_inference,
                AbsEnginePtr device_inferencer);

  static DeviceInferenceFactory &GetInstance();

  AbsEnginePtr GetDeviceInference(const std::string name, const std::string type);

  AbsEnginePtr
  SelectEnginebyType(const std::string name,
                    std::unordered_map<std::string, AbsEnginePtr> *engine_map);

private:
  std::unordered_map<std::string, AbsEnginePtr> trt_creator_;
  std::unordered_map<std::string, AbsEnginePtr> nb_creator_;
};

class DeviceRegister {
public:
  DeviceRegister(const std::string &name_inference, const std::string &type_inference,
                 AbsEnginePtr device_inferencer) {
    DeviceInferenceFactory::GetInstance().Register(
                             name_inference, type_inference, device_inferencer);
  }
  ~DeviceRegister() = default;
};

#define REG_DeviceEngine(name, type, inferencer) \
    DeviceRegister g_##name##_##type##_Engine_Reg(#name, #type, inferencer);
}
#endif // SELF_ARCHITECTURE_DEVICE_REGISTER_H
