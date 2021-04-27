//
// Created by yankai.yan on 2021/4/13.
//

#ifndef SELF_ARCHITECTURE_YOLO_ENGINE_INFER_H
#define SELF_ARCHITECTURE_YOLO_ENGINE_INFER_H

#include "amlogical_engine_infer.h"
namespace device {
class YoloEngineInfer : public AmlogicEngine {
 public:
  YoloEngineInfer(ModelCfgPtr model_cfg_ptr) : AmlogicEngine(model_cfg_ptr){};
  ~YoloEngineInfer() = default;
  vsi_nn_graph_t *InitModel(std::string binary_path);
};
}  // namespace device

#endif  // SELF_ARCHITECTURE_YOLO_ENGINE_INFER_H
