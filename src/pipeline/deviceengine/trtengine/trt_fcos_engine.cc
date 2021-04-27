//
// Created by yankai.yan on 2021/4/9.
//

#include "trt_fcos_engine.h"
#include "glog/logging.h"
#include "../device_register.h"
namespace device {
int TRTFcosEngine::CreateGraph(const contextPtr &cur_context_ptr) {
  LOG(ERROR) << "CREATE TRT GRAPH";
  return 0;
};
int TRTFcosEngine::RunProcess(const contextPtr &cur_context_ptr) {
  LOG(ERROR) << "RUN TRT GRAPH";
  return 0;
};
REG_DeviceEngine(fcos, trt, std::make_shared<TRTFcosEngine>(nullptr))
}  // namespace device
