//
// Created by yankai.yan on 2021/3/18.
//
#include "amlogical_engine_infer.h"
#include <string>
#include <unordered_map>
#include <memory>
#include "glog/logging.h"
#include "vnn_common/vnn_model.h"
#include "vnn_common/vnn_pre_process.h"
#include "vnn_common/vnn_global.h"
#include "vnn_common/vnn_post_process.h"


namespace device {
int AmlogicEngine::CreateGraph() {
  std::string binary_path = model_cfg_->model_binary_;
  if(binary_path.empty()) {
  LOG(ERROR) << "Create graph according binary path fail, due to binary path is invalid.";
  return -1;
  }
  vx_uint64 tms_start, tms_end, ms_val;
  tms_start = get_perf_count();
  self_graph_ = InitModel(binary_path);
  vsi_status status = vnn_VerifyGraph(self_graph_);
  if (status != VSI_SUCCESS) {
    LOG(ERROR) << "vnn_VerifyGraph failed";
    return -1;
  }
  for(uint32_t idx = 0; idx < self_graph_->output.num; ++idx) {
//    TensorInfo tensor_info;
    vsi_nn_tensor_t * out_tensor = ::vsi_nn_GetTensor(self_graph_, self_graph_->output.tensors[idx]);
//    tensor_info.element_sum = vsi_nn_GetElementNum(out_tensor);
//    tensor_info.tensor_data = new float[tensor_info.element_sum];
//    if (tensor_info.tensor_data == nullptr) {
//      LOG(ERROR) << "Parse output tensor info failed, due to new array is nullptr！";
//      return -1;
//    }
//    output_tensor_info_.emplace_back(std::move(tensor_info));
  }
  tms_end = get_perf_count();
  ms_val = (tms_end - tms_start) / 1000000;
  LOG(INFO) << "Create neural network:" << ms_val << "ms";
  return 0;
}

int AmlogicEngine::RunProcess() {
//  vsi_status status = VSI_FAILURE;
//  vx_uint64 tms_start, tms_end, ms_val;
//
//  tms_start = get_perf_count();
//  status = vsi_nn_RunGraph(self_graph_);
//  if (status != VSI_SUCCESS) {
//    LOG(ERROR) << "Run graph failed.";
//    return -1;
//  }
//  tms_end = get_perf_count();
//  ms_val = (tms_end - tms_start) / 1000000;
//  LOG(INFO) << "Run neural network cost: " << ms_val << "ms";
  return 0;
}
}