//
// Created by yankai.yan on 2021/4/9.
//

#ifndef SELF_ARCHITECTURE_TRT_FCOS_ENGINE_H
#define SELF_ARCHITECTURE_TRT_FCOS_ENGINE_H

#include "trt_engine.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace device {
class TRTFcosEngine : public TRTEngine {
 public:
  TRTFcosEngine(ModelCfgPtr model_cfg_ptr) : TRTEngine(model_cfg_ptr){};
  ~TRTFcosEngine() = default;
  int CreateGraph();
  int RunProcess();
  void SetModelCfg(ModelCfgPtr model_cfg_ptr) { model_cfg_ = model_cfg_ptr; }
};
}  // namespace device
#endif  // SELF_ARCHITECTURE_TRT_FCOS_ENGINE_H
