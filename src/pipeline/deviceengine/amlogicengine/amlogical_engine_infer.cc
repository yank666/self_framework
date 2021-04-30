//
// Created by yankai.yan on 2021/3/18.
//
#include "amlogical_engine_infer.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <iterator>
#include "glog/logging.h"
#include "src/utils/base.h"
#include "src/pipeline/deviceengine/amlogicengine/vnn_common/vnn_model.h"
#include "src/pipeline/deviceengine/amlogicengine/vnn_common/vnn_pre_process.h"

namespace device {
AmlogicEngine::~AmlogicEngine() {
}

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
  cur_context_ptr->out_dataflow_.resize(out_num_graph);
  for (uint32_t idx = 0; idx < out_num_graph; ++idx) {
    vsi_nn_tensor_t *out_tensor =
      ::vsi_nn_GetTensor(self_graph_, self_graph_->output.tensors[idx]);
    uint32_t element_sum = vsi_nn_GetElementNum(out_tensor);
    cur_context_ptr->out_dataflow_[idx].resize(element_sum);
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
  LOG(INFO) << "Before Run Yolo pre";
  PreProcess(cur_context_ptr);
  tms_start = get_perf_count();
  status = vsi_nn_RunGraph(self_graph_);
  if (status != VSI_SUCCESS) {
    LOG(ERROR) << "Run graph failed.";
    return -1;
  }
  tms_end = get_perf_count();
  ms_val = (tms_end - tms_start) / 1000000;
  LOG(INFO) << "Run " << model_cfg_->model_name_
            << "neural network cost: " << ms_val << "ms";
  PostProcess(cur_context_ptr);
  return 0;
}

int AmlogicEngine::PreProcess(const contextPtr &cur_context_ptr) {
  REPORT_ERROR_IF_NULL(cur_context_ptr);
  vx_uint64 tms_start, tms_end, ms_val;
  vsi_status status = VSI_FAILURE;
  tms_start = get_perf_count();
  uint32_t param_mean_size = model_cfg_->model_mean_.size();
  uint32_t param_norm_size = model_cfg_->model_norm_.size();
  if (param_mean_size != 3 || param_norm_size != 1) {
    LOG(ERROR) << "Input model_cfg config error, " << model_cfg_->model_name_
               << "has error mean or norm amount";
    return -1;
  }

  vsi_nn_tensor_t *tensor;
  tensor = vsi_nn_GetTensor( self_graph_, self_graph_->input.tensors[0] );
  uint32_t sz = vsi_nn_GetElementNum(tensor);
  LOG(ERROR) << "Preprocess sz." << sz;
  uint32_t stride = vsi_nn_TypeGetBytes(tensor->attr.dtype.vx_type);
  LOG(ERROR) << "Preprocess stride" << stride;
  uint8_t *dtype_data = (uint8_t *)malloc(stride * sz * sizeof(uint8_t));
  float *input_data = (float *)malloc(sz * sizeof(float));
  float *transform_data = (float *)malloc(sz * sizeof(float));
  status = vnn_PreProcessByPixels(self_graph_, cur_context_ptr->dataflow_[0].data(),
                                  input_data, transform_data, dtype_data);

  if (status != VSI_SUCCESS) {
    LOG(ERROR) << "Preprocess image data failed.";
    return -1;
  };
  tms_end = get_perf_count();
  ms_val = (tms_end - tms_start) / 1000000;
  LOG(INFO) << "Run " << model_cfg_->model_name_
            << "preprocess cost: " << ms_val << "ms";

  return 0;
}

int AmlogicEngine::PostProcess(const contextPtr &cur_context_ptr) {
  REPORT_ERROR_IF_NULL(cur_context_ptr);
  vx_uint64 tms_start, tms_end, ms_val;
  tms_start = get_perf_count();
  uint32_t out_num_graph = self_graph_->output.num;
  cur_context_ptr->out_shape_.resize(out_num_graph);
  for (int i = 0; i < out_num_graph; i++) {
    uint32_t sz = 1, stride;
    uint8_t *tensor_data = NULL;

        vsi_nn_tensor_t *out_tensor_t =
      vsi_nn_GetTensor(self_graph_, self_graph_->output.tensors[i]);
    for (auto dim : out_tensor_t->attr.size) {
      cur_context_ptr->out_shape_[i].emplace_back(dim);
    }
    float *buffer = cur_context_ptr->out_dataflow_[i].data();
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
  }

//  for (uint32_t idx = 0; idx < out_num_graph; ++idx) {
//    vsi_nn_tensor_t *out_tensor_t =
//      vsi_nn_GetTensor(self_graph_, self_graph_->output.tensors[idx]);
//    for (auto dim : out_tensor_t->attr.size) {
//      cur_context_ptr->out_shape_[idx].emplace_back(dim);
//    }
//    uint8_t *out_tensor_data =
//      (uint8_t *)vsi_nn_ConvertTensorToData(self_graph_, out_tensor_t);
//    uint32_t tensor_stride =
//      ::vsi_nn_TypeGetBytes(out_tensor_t->attr.dtype.vx_type);
//
//    auto iter_begin = cur_context_ptr->out_dataflow_[idx].begin();
//    auto iter_end = cur_context_ptr->out_dataflow_[idx].end();
//    uint32_t iter_poisition = 0;
//    std::for_each(iter_begin, iter_end, [&](float &iter) {
//      vsi_status status =
//        vsi_nn_DtypeToFloat32(&(out_tensor_data[tensor_stride * iter_poisition]), &(iter),
//                              &out_tensor_t->attr.dtype);
//      iter_poisition++;
//      if (status == VSI_FAILURE) {
//        LOG(ERROR) << "vsi_nn_DtypeToFloat32 run faied !";
//        vsi_nn_Free(out_tensor_data);
//      }
//    });
//    vsi_nn_Free(out_tensor_data);
//  }
  tms_end = get_perf_count();
  ms_val = (tms_end - tms_start) / 1000000;
  LOG(INFO) << "Run " << model_cfg_->model_name_
            << "postprocess cost: " << ms_val << "ms";
  return 0;
}
}  // namespace device