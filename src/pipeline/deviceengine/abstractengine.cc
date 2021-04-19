//
// Created by yankai.yan on 2021/3/18.
//

#include "abstractengine.h"
#include "glog/logging.h"
namespace device {
bool AbstractEngine::SetModelCfg(device::ModelCfgPtr model_cfg_ptr) {
  if (model_cfg_ptr == nullptr) {
    //    LOG(ERROR) << "Engine Set modelcfg fail, because model_cfg_ptr is
    //    nullptr";
    return false;
  }
  model_cfg_ = model_cfg_ptr;
  return true;
}
}  // namespace device