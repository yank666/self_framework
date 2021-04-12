//
// Created by yankai.yan on 2021/3/18.
//

#include "reid_engine_infer.h"
#include <memory>
#include <string>
#include "../device_register.h"
namespace device {
int ReidEngineInfer::CreateGraph() {

  return 0;
}

int ReidEngineInfer::RunProcess() {
  return 0;
}
REG_DeviceEngine(reid, nb, std::make_shared<ReidEngineInfer>(nullptr))
}