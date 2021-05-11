//
// Created by yankai.yan on 2021/3/18.
//

#include "abstractengine.h"
#include <algorithm>
#include "glog/logging.h"
#include "base.h"
namespace device {

void Context::TransmitContext(const std::shared_ptr<Context> dst_contest) {
  REPORT_EXCEPTION_IF_NULL(dst_contest);
  dst_contest->ori_data = ori_data;
  dst_contest->input_w = input_w;
  dst_contest->input_h = input_h;
  dst_contest->channel_ = channel_;

  dst_contest->dataflow_.clear();
  dst_contest->dataflow_.resize(out_dataflow_.size());

  for (int i = 0; i < out_dataflow_.size(); i++) {
    dst_contest->dataflow_[i].resize(out_dataflow_[i].size());
    std::memcpy(dst_contest->dataflow_[i].data(), out_dataflow_[i].data(),
                out_dataflow_[i].size() * sizeof(float));
  }
  dst_contest->in_shape_ = out_shape_;
}
void Context::CopyContext(const std::shared_ptr<Context> dst_contest) {
  REPORT_EXCEPTION_IF_NULL(dst_contest);
  dst_contest->ori_data = ori_data;
  dst_contest->input_w = input_w;
  dst_contest->input_h = input_h;
}

bool AbstractEngine::SetModelCfg(device::ModelCfgPtr model_cfg_ptr) {
  if (model_cfg_ptr == nullptr) {
    LOG(ERROR) << "Engine Set modelcfg fail, because model_cfg_ptr is nullptr";
    return false;
  }
  model_cfg_ = model_cfg_ptr;
  return true;
}
}  // namespace device