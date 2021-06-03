//
// Created by yankai.yan on 2021/3/18.
//
#include "amlogical_engine_infer.h"
#include <memory>
#include <string>
#include <algorithm>
#include <iterator>
#include <numeric>
#include "glog/logging.h"
#include "src/utils/base.h"
#include "src/pipeline/deviceengine/amlogicengine/vnn_common/vnn_model.h"
#include "src/pipeline/deviceengine/amlogicengine/vnn_common/vnn_pre_process.h"

namespace device {
AmlogicEngine::~AmlogicEngine() {}

int AmlogicEngine::CreateGraph(const contextPtr &cur_context_ptr) {
  REPORT_ERROR_IF_NULL(cur_context_ptr);
  std::string binary_path = model_cfg_->model_binary_;
  if (binary_path.empty()) {
    LOG(ERROR) << "Create graph according binary path fail, due to binary path "
                  "is invalid.";
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
  uint32_t out_num_graph = self_graph_->output.num;
  cur_context_ptr->out_shape_.resize(out_num_graph);
  for (uint32_t idx = 0; idx < out_num_graph; ++idx) {
    vsi_nn_tensor_t *out_tensor =
      ::vsi_nn_GetTensor(self_graph_, self_graph_->output.tensors[idx]);
    element_sum_ += vsi_nn_GetElementNum(out_tensor);
    for (auto dim : out_tensor->attr.size) {
      cur_context_ptr->out_shape_[idx].emplace_back(dim);
    }
  }

  tms_end = get_perf_count();
  ms_val = (tms_end - tms_start) / 1000000;
  LOG(INFO) << "Create " << model_cfg_->model_name_
            << "neural network:" << ms_val << "ms";
  return 0;
}

int AmlogicEngine::RunProcess(const contextPtr &cur_context_ptr) {
  vsi_status status = VSI_FAILURE;
  vx_uint64 tms_start, tms_end, ms_val;
  tms_start = get_perf_count();
  cur_context_ptr->out_dataflow_.clear();
  uint32_t loops = cur_context_ptr->batches;
  if (loops != 0) {
    cur_context_ptr->out_dataflow_.resize(loops);
  } else {
    cur_context_ptr->out_dataflow_.resize(1);
    loops = 1;
  }
  std::for_each(
    cur_context_ptr->out_dataflow_.begin(),
    cur_context_ptr->out_dataflow_.end(),
    [&](std::vector<char> &vec) { vec.resize(element_sum_ * sizeof(float)); });
  uint32_t count = 0;
  while (count < loops) {
    PreProcess(cur_context_ptr, count);
    status = vsi_nn_RunGraph(self_graph_);
    if (status != VSI_SUCCESS) {
      LOG(ERROR) << "Run graph failed.";
      return -1;
    }
    PostProcess(cur_context_ptr, count);
    count++;
  }
  tms_end = get_perf_count();
  ms_val = (tms_end - tms_start) / 1000000;
  LOG(INFO) << "Run " << model_cfg_->model_name_
            << " neural network cost: " << ms_val << "ms";
  return 0;
}

int AmlogicEngine::PreProcess(const contextPtr &cur_context_ptr,
                              const uint32_t &idx) {
  REPORT_ERROR_IF_NULL(cur_context_ptr);
  vx_uint64 tms_start = get_perf_count();
  uint32_t param_mean_size = model_cfg_->model_mean_.size();
  uint32_t param_norm_size = model_cfg_->model_norm_.size();
  if (param_mean_size != 3 || param_norm_size != 1) {
    LOG(ERROR) << "Input model_cfg config error, " << model_cfg_->model_name_
               << "has error mean or norm amount";
    return -1;
  }
  uint32_t channel_format = 0;
  if (model_cfg_->data_format_ == "RGB") {
    channel_format = 0;
  } else if (model_cfg_->data_format_ == "BGR") {
    channel_format = 1;
  } else {
    LOG(ERROR) << "Input channel format not support yet, "
               << model_cfg_->data_format_;
  }
  vnn_SetChannelandMean(channel_format, model_cfg_->model_mean_.data(),
                        model_cfg_->model_norm_.data());
  vsi_nn_tensor_t *tensor;
  tensor = vsi_nn_GetTensor(self_graph_, self_graph_->input.tensors[0]);
  uint32_t sz = vsi_nn_GetElementNum(tensor);
  uint32_t stride = vsi_nn_TypeGetBytes(tensor->attr.dtype.vx_type);
  uint8_t *dtype_data = (uint8_t *)malloc(stride * sz * sizeof(uint8_t));
  REPORT_ERROR_IF_NULL(dtype_data);
  float *input_data = (float *)malloc(sz * sizeof(float));
  REPORT_ERROR_IF_NULL(input_data);
  float *transform_data = (float *)malloc(sz * sizeof(float));
  REPORT_ERROR_IF_NULL(transform_data);
  vsi_status status =
    vnn_PreProcessByPixels(self_graph_, cur_context_ptr->dataflow_[0].data(),
                           input_data, transform_data, dtype_data);
  if (status != VSI_SUCCESS) {
    LOG(ERROR) << "Preprocess image data failed.";
    return -1;
  };
  vx_uint64 tms_end = get_perf_count();
  vx_uint64 ms_val = (tms_end - tms_start) / 1000000;
  LOG(INFO) << "Run " << model_cfg_->model_name_
            << "preprocess cost: " << ms_val << "ms";
  free(dtype_data);
  free(input_data);
  free(transform_data);
  return 0;
}

int AmlogicEngine::PostProcess(const contextPtr &cur_context_ptr,
                               const uint32_t &idx) {
  REPORT_ERROR_IF_NULL(cur_context_ptr);
  vx_uint64 tms_start, tms_end, ms_val;
  tms_start = get_perf_count();
  uint32_t out_num_graph = self_graph_->output.num;
  uint64_t cur_position = 0;
  for (int i = 0; i < out_num_graph; i++) {
    uint32_t sz = 1, stride;
    uint8_t *tensor_data = NULL;

    float *buffer =
      reinterpret_cast<float *>(cur_context_ptr->out_dataflow_[idx].data()) +
      cur_position;
    float scale;
    int32_t zero_point;
    float fl;
    vsi_nn_tensor_t *o_tensor =
      vsi_nn_GetTensor(self_graph_, self_graph_->output.tensors[i]);
    if (o_tensor->attr.dtype.vx_type == VSI_NN_TYPE_UINT8) {
      scale = o_tensor->attr.dtype.scale;
      zero_point = o_tensor->attr.dtype.zero_point;
    } else {
      fl = pow(2., -o_tensor->attr.dtype.fl);
    }

    for (int j = 0; j < o_tensor->attr.dim_num; j++) {
      sz *= o_tensor->attr.size[j];
    }
    stride = vsi_nn_TypeGetBytes(o_tensor->attr.dtype.vx_type);
    tensor_data = (uint8_t *)vsi_nn_ConvertTensorToData(self_graph_, o_tensor);
    if (buffer == NULL) {
      LOG(INFO) << "break";
      break;
    }
    for (int j = 0; j < sz; j++) {
      switch (o_tensor->attr.dtype.vx_type) {
        case VSI_NN_TYPE_UINT8:
          buffer[j] =
            (*(uint8_t *)&tensor_data[stride * j] - zero_point) * scale;
          break;
        case VSI_NN_TYPE_INT8:
          buffer[j] = *(int8_t *)&tensor_data[stride * j] * fl;
          break;
        case VSI_NN_TYPE_INT16:
          buffer[j] = *(int16_t *)&tensor_data[stride * j] * fl;
          break;
        case VSI_NN_TYPE_FLOAT16: {
          vx_int16 data = *(vx_int16 *)(&tensor_data[stride * j]);
          vx_int32 t1 = data & 0x7FFF;
          vx_int32 t2 = data & 0x8000;
          vx_int32 t3 = data & 0x7C00;
          vx_float32 out;

          t1 <<= 13;
          t2 <<= 16;

          t1 += 0x38000000;
          t1 = (t3 == 0 ? 0 : t1);
          t1 |= t2;
          *((uint32_t *)&out) = t1;

          buffer[j] = out;
          break;
        }
        case VSI_NN_TYPE_FLOAT32:
          buffer[j] = *(float *)(&tensor_data[stride * j]);
          break;
        default:
          break;
      }
    }
    if (tensor_data) {
      free(tensor_data);
    }
    cur_position += std::accumulate(std::begin(cur_context_ptr->out_shape_[i]),
                                    std::end(cur_context_ptr->out_shape_[i]), 0,
                                    std::plus<uint32_t>());
  }
  tms_end = get_perf_count();
  ms_val = (tms_end - tms_start) / 1000000;
  LOG(INFO) << "Run " << model_cfg_->model_name_
            << "postprocess cost: " << ms_val << "ms";
  return 0;
}
}  // namespace device