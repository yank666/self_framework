//
// Created by yankai.yan on 2021/4/9.
//

#include "trt_reid_engine.h"
#include "glog/logging.h"
#include "../device_register.h"
#include "opencv2/opencv.hpp"
namespace device {
int TRTReidEngine::CreateGraph(const contextPtr &cur_context_ptr) {
  DLOG(INFO) << "CREATE TRT GRAPH";
  return 0;
};
int TRTReidEngine::RunProcess(const contextPtr &cur_context_ptr) {
  DLOG(INFO) << "RUN TRT GRAPH";
  return 0;
};
REG_DeviceEngine(reid, trt, std::make_shared<TRTReidEngine>(nullptr))
}  // namespace device
