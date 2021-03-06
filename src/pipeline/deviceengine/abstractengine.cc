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
  dst_contest->batches = batches;
  dst_contest->stream_id_ = stream_id_;
  dst_contest->equipment_id_ = equipment_id_;
  dst_contest->time_stamp_ = time_stamp_;
  dst_contest->frame_num_ = frame_num_;

  dst_contest->dataflow_.clear();
  dst_contest->dataflow_.resize(out_dataflow_.size());

  for (int i = 0; i < out_dataflow_.size(); i++) {
    dst_contest->dataflow_[i].resize(out_dataflow_[i].size());
    std::memcpy(dst_contest->dataflow_[i].data(), out_dataflow_[i].data(),
                out_dataflow_[i].size());
  }
  dst_contest->in_shape_ = out_shape_;
}
void Context::CopyContext(const std::shared_ptr<Context> dst_contest) {
  REPORT_EXCEPTION_IF_NULL(dst_contest);
  dst_contest->ori_data = ori_data;
  dst_contest->input_w = input_w;
  dst_contest->input_h = input_h;
}

void Context::SetHardwareInfo(const std::string &stream_id,
                              const std::string &eq_id, const int &image_id,
                              const long &timestap) {
  stream_id_ = stream_id;
  equipment_id_ = eq_id;
  frame_num_ = image_id;
  time_stamp_ = timestap;
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