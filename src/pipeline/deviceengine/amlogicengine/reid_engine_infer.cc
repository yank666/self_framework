//
// Created by yankai.yan on 2021/3/18.
//

#include "reid_engine_infer.h"
#include <string>
#include "src/pipeline/deviceengine/amlogicengine/vnn_common/vnn_model.h"
#include "src/pipeline/deviceengine/amlogicengine/vnn_common/vnn_post_process.h"
#include "src/pipeline/deviceengine/amlogicengine/vnn_common/vnn_pre_process.h"
#include "src/pipeline/deviceengine/device_register.h"

namespace device {
vsi_nn_graph_t *ReidEngineInfer::InitModel(std::string binary_path) {
  return vnn_CreateEdge256PedReid(
    reinterpret_cast<const char *>(binary_path.data()), NULL,
    vnn_GetPrePorcessMap(), vnn_GetPrePorcessMapCount(),
    vnn_GetPostPorcessMap(), vnn_GetPostPorcessMapCount());
}
REG_DeviceEngine(reid, nb, std::make_shared<ReidEngineInfer>(nullptr))
}  // namespace device