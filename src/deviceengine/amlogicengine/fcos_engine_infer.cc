//
// Created by yankai.yan on 2021/3/18.
//

#include "fcos_engine_infer.h"
#include <memory>
#include <string>
#include "../device_register.h"
namespace device {
int FcosEngineInfer::CreateGraph() {

  return 0;
}

int FcosEngineInfer::RunProcess() {
  return 0;
}
REG_DeviceEngine(fcos, nb, std::make_shared<FcosEngineInfer>())
}