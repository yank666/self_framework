//
// Created by yankai.yan on 2021/4/13.
//

#include "yolo_engine_infer.h"
#include "glog/logging.h"
#include "src/pipeline/deviceengine/amlogicengine/vnn_common/vnn_global.h"
#include "src/pipeline/deviceengine/amlogicengine/vnn_common/vnn_model.h"
#include "src/pipeline/deviceengine/amlogicengine/vnn_common/vnn_post_process.h"
#include "src/pipeline/deviceengine/amlogicengine/vnn_common/vnn_pre_process.h"
#include "src/pipeline/deviceengine/device_register.h"
#include <string>

namespace device {
vsi_nn_graph_t *YoloEngineInfer::InitModel(std::string binary_path) {
  return vnn_CreateYolov5Sim(
    reinterpret_cast<const char *>(binary_path.data()), NULL,
    vnn_GetPrePorcessMap(), vnn_GetPrePorcessMapCount(),
    vnn_GetPostPorcessMap(), vnn_GetPostPorcessMapCount());
}

REG_DeviceEngine(yolo, nb, std::make_shared<YoloEngineInfer>(nullptr))
}  // namespace device