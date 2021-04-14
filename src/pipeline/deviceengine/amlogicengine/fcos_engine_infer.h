//
// Created by yankai.yan on 2021/3/18.
//

#ifndef SELF_ARCHITECTURE_FCOS_ENGINE_INFER_H
#define SELF_ARCHITECTURE_FCOS_ENGINE_INFER_H
#include "amlogical_engine_infer.h"
namespace device {
class FcosEngineInfer : public AmlogicEngine {
public:
  FcosEngineInfer(ModelCfgPtr model_cfg_ptr) : AmlogicEngine( model_cfg_ptr) {};
  ~FcosEngineInfer() = default;
//  int CreateGraph();
//  int RunProcess();
  vsi_nn_graph_t *InitModel(std::string binary_path);
};
}
#endif // SELF_ARCHITECTURE_FCOS_ENGINE_INFER_H
