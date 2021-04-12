//
// Created by yankai.yan on 2021/3/18.
//

#ifndef SELF_ARCHITECTURE_AMLOGICAL_ENGINE_INFER_H
#define SELF_ARCHITECTURE_AMLOGICAL_ENGINE_INFER_H

#include <string>
#include "../abstractengine.h"
#include "vnn_common/vnn_model.h"

namespace device {
class AmlogicEngine : public AbstractEngine {
public:
  AmlogicEngine(ModelCfgPtr model_cfg_ptr) : AbstractEngine(model_cfg_ptr){}
   ~AmlogicEngine() = default;
  int CreateGraph();
  int RunProcess();
  virtual vsi_nn_graph_t *InitModel(std::string binary_path) {return nullptr;}

protected:
  vsi_nn_graph_t *self_graph_ = NULL;
};
}
#endif // SELF_ARCHITECTURE_AMLOGICAL_ENGINE_INFER_H
