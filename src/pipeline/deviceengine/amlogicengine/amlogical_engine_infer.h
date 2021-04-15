//
// Created by yankai.yan on 2021/3/18.
//

#ifndef SELF_ARCHITECTURE_AMLOGICAL_ENGINE_INFER_H
#define SELF_ARCHITECTURE_AMLOGICAL_ENGINE_INFER_H

#include "src/pipeline/deviceengine/abstractengine.h"
#include "src/pipeline/deviceengine/amlogicengine/vnn_common/vnn_model.h"
#include <string>

namespace device {
class AmlogicEngine : public AbstractEngine {
public:
  AmlogicEngine(ModelCfgPtr model_cfg_ptr) : AbstractEngine(model_cfg_ptr){}
   ~AmlogicEngine();
  int CreateGraph(const contextPtr& cur_context_ptr);
  int RunProcess(const contextPtr& cur_context_ptr);
  virtual vsi_nn_graph_t *InitModel(std::string binary_path) {return nullptr;}
  int PreProcess(const contextPtr& cur_context_ptr);
  int PostProcess(const contextPtr& cur_context_ptr);
protected:
  vsi_nn_graph_t *self_graph_ = NULL;
};
}
#endif // SELF_ARCHITECTURE_AMLOGICAL_ENGINE_INFER_H