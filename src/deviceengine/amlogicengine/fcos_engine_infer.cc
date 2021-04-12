//
// Created by yankai.yan on 2021/3/18.
//

#include "fcos_engine_infer.h"
#include <memory>
#include <string>
#include "glog/logging.h"
#include "../device_register.h"
#include "vnn_common/vnn_global.h"
#include "vnn_common/vnn_pre_process.h"
#include "vnn_common/vnn_model.h"
#include "vnn_common/vnn_post_process.h"

namespace device {
//int FcosEngineInfer::CreateGraph() {
//
//  return 0;
//}
//
//int FcosEngineInfer::RunProcess() {
//  return 0;
//}
vsi_nn_graph_t *FcosEngineInfer::InitModel(std::string binary_path) {
  return vnn_CreateEdge256PedReid(reinterpret_cast<const char *>(binary_path.data()), NULL,
                                   vnn_GetPrePorcessMap(), vnn_GetPrePorcessMapCount(),
                                   vnn_GetPostPorcessMap(), vnn_GetPostPorcessMapCount());
}

REG_DeviceEngine(fcos, nb, std::make_shared<FcosEngineInfer>(nullptr))
}