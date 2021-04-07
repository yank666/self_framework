//
// Created by yankai.yan on 2021/3/18.
//

#ifndef SELF_ARCHITECTURE_TRT_ENGINE_INFER_H
#define SELF_ARCHITECTURE_TRT_ENGINE_INFER_H

#include <string>
#include "../abstractengine.h"
namespace device {
class TRTEngine : public AbstractEngine {
public:
  TRTEngine(ModelCfgPtr model_cfg_ptr) : AbstractEngine(model_cfg_ptr){}
   ~TRTEngine() = default;
  int CreateGraph() {return 0;};
  int RunProcess() {return 0;};

protected:
  //  vsi_nn_graph_t *self_graph_ = NULL;
};
}
#endif // SELF_ARCHITECTURE_TRT_ENGINE_INFER_H
