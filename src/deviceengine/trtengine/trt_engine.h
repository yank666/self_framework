//
// Created by yankai.yan on 2021/3/18.
//

#ifndef SELF_ARCHITECTURE_TRT_ENGINE_H
#define SELF_ARCHITECTURE_TRT_ENGINE_H

#include "../abstractengine.h"
namespace device {
class TRTEngine : public AbstractEngine {
public:
  TRTEngine(ModelCfgPtr model_cfg_ptr) : AbstractEngine(model_cfg_ptr){}
  virtual ~TRTEngine() = default;
  virtual int CreateGraph();
  virtual int RunProcess();
};
}
#endif // SELF_ARCHITECTURE_TRT_ENGINE_H
