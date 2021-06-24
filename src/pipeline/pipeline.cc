#include "pipeline.h"
#include <algorithm>
#include <iostream>
#include <numeric>
#include "glog/logging.h"
#include "src/utils/base.h"
#include "stage_register.h"
#include "src/utils/errorcode.h"
#include "deviceengine/device_register.h"

namespace pipeline {
enum ContextIdx : uint32_t { datainput = 0, dataoutput = 1, dataidx = 2 };

void Pipeline::RegisterStage(const ModelCfgPtr &model_cfg) {
  contextPtr stage_context = std::make_shared<device::Context>();
  REPORT_EXCEPTION_IF_NULL(stage_context);

  uint32_t current_stage_pos = model_cfg->model_position_;
  std::string cur_stage_name = model_cfg->model_name_;
  DecorStagePtr exec_stage_ptr =
    DeviceInferenceFactory::GetInstance().GetDeviceInference(cur_stage_name);
  REPORT_EXCEPTION_IF_NULL(exec_stage_ptr);
  exec_stage_ptr->SetStageName(model_cfg->model_name_);
  exec_stage_ptr->AddProcess(nullptr);
  stages_[current_stage_pos].push_back(exec_stage_ptr);
  process_context_.insert(std::make_pair(
    cur_stage_name, std::make_pair(exec_stage_ptr, stage_context)));

  if (current_stage_pos != 0) {
    for (auto &stage : stages_[current_stage_pos - 1]) {
      stage->ConnectTailStage(cur_stage_name);
    }
  }
}

void Pipeline::RegisterEngine(const ModelCfgPtr &model_cfg) {
  DeviceStagePtr model_inference_ptr = std::make_shared<DeviceStage>(model_cfg);
  REPORT_EXCEPTION_IF_NULL(model_inference_ptr);
  contextPtr stage_context = std::make_shared<device::Context>();
  REPORT_EXCEPTION_IF_NULL(stage_context);

  model_inference_ptr->FillStagebyEngine(stage_context);
  uint32_t current_model_pos = model_cfg->model_position_;
  CreateContexts(stage_context, model_cfg);

  std::string cur_model_name = model_cfg->model_name_;
  DecorStagePtr exec_stage_ptr =
    DeviceInferenceFactory::GetInstance().GetDeviceInference(cur_model_name);
  if (exec_stage_ptr != nullptr) {
    exec_stage_ptr->AddProcess(model_inference_ptr);
    stages_[current_model_pos].push_back(exec_stage_ptr);
    process_context_.insert(std::make_pair(
      cur_model_name, std::make_pair(exec_stage_ptr, stage_context)));
  } else {
    stages_[current_model_pos].push_back(model_inference_ptr);
    process_context_.insert(std::make_pair(
      cur_model_name, std::make_pair(model_inference_ptr, stage_context)));
  }
  if (current_model_pos != 0) {
    for (auto &stage : stages_[current_model_pos - 1]) {
      stage->ConnectTailStage(cur_model_name);
    }
  }
}

bool Pipeline::CreateContexts(const contextPtr &cur_context_ptr,
                              const ModelCfgPtr &cur_model_cfg) {
  REPORT_ERROR_IF_NULL(cur_context_ptr);
  REPORT_ERROR_IF_NULL(cur_model_cfg);
  cur_context_ptr->name_ = cur_model_cfg->model_name_;

  uint32_t input_cnt = cur_model_cfg->model_inshape_.size();
  cur_context_ptr->in_shape_.resize(input_cnt);
  cur_context_ptr->dataflow_.resize(input_cnt);
  uint32_t input_sum = 1;
  for (auto &inshape : cur_model_cfg->model_inshape_) {
    uint32_t input_sum_temp = std::accumulate(
      std::begin(inshape), std::end(inshape), 1, std::multiplies<>());
    input_sum *= input_sum_temp;
    cur_context_ptr->dataflow_[datainput].resize(input_sum);
    std::vector<uint32_t> temp_vec;
    std::for_each(inshape.begin(), inshape.end(),
                  [&](const uint32_t &dim) { temp_vec.push_back(dim); });
    cur_context_ptr->in_shape_.push_back(temp_vec);
  }
  return true;
}

bool Pipeline::InitPipeline(const std::vector<ModelCfgPtr> &model_cfgs) {
  if (model_cfgs.empty()) {
    LOG(ERROR) << "Pipeline init failed, model cfgs is emtpy!";
    return false;
  }
  stages_.resize(model_cfgs.size());
  for (const auto &model_cfg : model_cfgs) {
    if (model_cfg->is_inferengine_) {
      RegisterEngine(model_cfg);
    } else {
      RegisterStage(model_cfg);
    }
  }
  is_ready_.store(true);
  return true;
}

void Pipeline::DeliveryContext(const AbstractStagePtr &cur_staga,
                               const contextPtr &cur_context) {
  REPORT_EXCEPTION_IF_NULL(cur_staga);
  REPORT_EXCEPTION_IF_NULL(cur_context);
  auto tail_stage_name = cur_staga->GetTailStage();
  if (tail_stage_name.empty()) {
    return;
  }
  for (const auto &stage_name : tail_stage_name) {
    auto tail_context = GetStageContextbyName(stage_name);
    cur_context->TransmitContext(tail_context);
  }
}

void Pipeline::RunPipeline() {
  std::for_each(stages_.begin(), stages_.end(),
                [&](const std::vector<AbstractStagePtr> &stage_vec) {
                  // TODO (yankai): consider add multithread exec staege ,if
                  // stage_vec size is more than 1;
                  for (auto &stage : stage_vec) {
                    auto cur_context_ptr =
                      GetStageContextbyName(stage->GetModelName());
                    REPORT_ERROR_IF_NULL(cur_context_ptr);
                    stage->RunStage(cur_context_ptr, process_context_);
                    DeliveryContext(stage, cur_context_ptr);
                  }
                  return true;
                });

  is_ready_.store(true);
}

bool Pipeline::PushDatatoPipeline(char **input_data,
                                  const std::vector<uint32_t> &input_size,
                                  const uint32_t &input_width,
                                  const uint32_t &input_height) {
  for (size_t i = 0; i < stages_[0].size(); ++i) {
    std::string stage_name = stages_[0][i]->GetModelName();
    auto stage_context_ptr = GetStageContextbyName(stage_name);
    if (stage_context_ptr == nullptr) {
      return false;
    }
    stage_context_ptr->batches = 1;
    stage_context_ptr->input_w = input_width;
    stage_context_ptr->input_h = input_height;

    stage_context_ptr->ori_data.clear();
    stage_context_ptr->ori_data.resize(stages_[0].size());
    stage_context_ptr->ori_data[i].resize(input_size[i]);
    memcpy(stage_context_ptr->ori_data[0].data(), input_data[i], input_size[i]);

    stage_context_ptr->dataflow_.clear();
    stage_context_ptr->dataflow_.resize(1);
    stage_context_ptr->dataflow_[0].resize(input_size[i]);
    memcpy(stage_context_ptr->dataflow_[0].data(), input_data[i],
           input_size[i]);
  }
  is_ready_.store(false);
  return RET_OK;
}

contextPtr Pipeline::GetStageContextbyName(const std::string &stage_name) {
  auto iter = process_context_.find(stage_name);
  if (iter != process_context_.end()) {
    const ProcessContext &process_ctx = iter->second;
    return process_ctx.second;
  }
  return nullptr;
}

void Pipeline::SetHardwareofContext(const std::string &stream_id,
                                    const std::string &eq_id,
                                    const int &image_id, const long &timestap) {
  for (auto &stage : *(stages_.begin())) {
    contextPtr ctx = GetStageContextbyName(stage->GetModelName());
    ctx->SetHardwareInfo(stream_id, eq_id, image_id, timestap);
  }
}

DeviceStage::DeviceStage(const ModelCfgPtr &model_cfg)
    : AbstractStage(model_cfg) {
  engine_ = device::DeviceEngineFactory::GetInstance().GetDeviceInference(
    model_cfg->model_name_, model_cfg->model_type_);
  REPORT_EXCEPTION_IF_NULL(engine_);
  engine_->SetModelCfg(model_cfg);
}

bool DeviceStage::FillStagebyEngine(const contextPtr &conext_ptr) {
  if (engine_ == nullptr) {
    LOG(ERROR) << "DeviceStage obtain engine is nullptr";
    return false;
  }
  REPORT_ERROR_IF_NULL(conext_ptr);
  engine_->CreateGraph(conext_ptr);
  return true;
}

bool DeviceStage::RunStage(const contextPtr &conext_ptr,
                           const ProcessContextMap &contextMap) {
  // TODO：add run graph
  engine_->RunProcess(conext_ptr);
  return true;
}

bool DecoratorStage::RunStage(const contextPtr &conext_ptr,
                              const ProcessContextMap &contextMap) {
  StagePreProcess(conext_ptr, contextMap);
  RunSubStage(conext_ptr, contextMap);
  StagePostProcess(conext_ptr, contextMap);
  return true;
}

bool DecoratorStage::AddProcess(const DeviceStagePtr &device_ptr) {
  REPORT_ERROR_IF_NULL(device_ptr);
  extra_stage_ptr_ = device_ptr;
  stage_name_ = device_ptr->GetModelName();
  return true;
}
}  // namespace pipeline