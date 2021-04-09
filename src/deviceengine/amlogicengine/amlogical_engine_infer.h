//
// Created by yankai.yan on 2021/3/18.
//

#ifndef SELF_ARCHITECTURE_AMLOGICAL_ENGINE_INFER_H
#define SELF_ARCHITECTURE_AMLOGICAL_ENGINE_INFER_H

#include <string>
#include "../abstractengine.h"
namespace device {
class AmlogicEngine : public AbstractEngine {
public:
  AmlogicEngine(ModelCfgPtr model_cfg_ptr) : AbstractEngine(model_cfg_ptr){}
   ~AmlogicEngine() = default;
  int CreateGraph();
  int RunProcess();

protected:
  //  vsi_nn_graph_t *self_graph_ = NULL;
};
}
#endif // SELF_ARCHITECTURE_AMLOGICAL_ENGINE_INFER_H
