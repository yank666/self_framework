//
// Created by yankai.yan on 2021/4/9.
//

#ifndef SELF_ARCHITECTURE_TRT_REID_ENGINE_H
#define SELF_ARCHITECTURE_TRT_REID_ENGINE_H

#include "trt_engine.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace device {
class TRTReidEngine : public TRTEngine {
public:
  TRTReidEngine(ModelCfgPtr model_cfg_ptr) : TRTEngine(model_cfg_ptr){};
  ~TRTReidEngine() = default;
  int CreateGraph();
  int RunProcess();
  void SetModelCfg(ModelCfgPtr model_cfg_ptr) {model_cfg_ = model_cfg_ptr;}
};
}
#endif // SELF_ARCHITECTURE_TRT_REID_ENGINE_H
