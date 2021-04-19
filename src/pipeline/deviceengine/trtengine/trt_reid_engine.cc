//
// Created by yankai.yan on 2021/4/9.
//

#include "trt_reid_engine.h"
#include "glog/logging.h"
#include "../device_register.h"
namespace device {
int TRTReidEngine::CreateGraph() {
  LOG(ERROR) << "CREATE TRT GRAPH";
  return 0;
};
int TRTReidEngine::RunProcess() {
  LOG(ERROR) << "RUN TRT GRAPH";
  return 0;
};
REG_DeviceEngine(reid, trt, std::make_shared<TRTReidEngine>(nullptr))
}  // namespace device
