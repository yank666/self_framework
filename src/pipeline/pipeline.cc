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
  DeviceStagePtr model_inference_ptr = std::make_shared<DeviceStage>(model_cfg);
  REPORT_EXCEPTION_IF_NULL(model_inference_ptr);
  contextPtr stage_context = std::make_shared<device::Context>();
  REPORT_EXCEPTION_IF_NULL(stage_context);

  CreateContexts(stage_context, model_cfg);
  model_inference_ptr->FillStagebyEngine(stage_context);
  DecorStagePtr exec_stage_ptr =
    DeviceInferenceFactory::GetInstance().GetDeviceInference(
      model_inference_ptr->GetModelName());
  if (exec_stage_ptr != nullptr) {
    exec_stage_ptr->AddProcess(model_inference_ptr);
    stages_[model_cfg->model_position_].push_back(exec_stage_ptr);
    process_context_.insert(std::make_pair(
      model_cfg->model_name_, std::make_pair(exec_stage_ptr, stage_context)));
  } else {
    stages_[model_cfg->model_position_].push_back(model_inference_ptr);
    process_context_.insert(
      std::make_pair(model_cfg->model_name_,
                     std::make_pair(model_inference_ptr, stage_context)));
  }
}

bool Pipeline::CreateContexts(const contextPtr &cur_context_ptr,
                              const ModelCfgPtr &cur_model_cfg) {
  REPORT_ERROR_IF_NULL(cur_context_ptr);
  REPORT_ERROR_IF_NULL(cur_model_cfg);
  cur_context_ptr->name_ = cur_model_cfg->model_name_;

  uint32_t input_cnt = cur_model_cfg->model_inshape_.size();
  cur_context_ptr->datasize_.resize(input_cnt);
  cur_context_ptr->dataflow_.resize(input_cnt);
  uint32_t input_sum = 1;
  for (auto &inshape : cur_model_cfg->model_inshape_) {
    uint32_t input_sum_temp = std::accumulate(
      std::begin(inshape), std::end(inshape), 1, std::multiplies<uint32_t>());
    input_sum *= input_sum_temp;
    cur_context_ptr->datasize_[datainput] = input_sum;
    cur_context_ptr->dataflow_[datainput].resize(input_sum);
  }

  //  uint32_t output_sum = 1;
  //  for (auto &outshape : cur_model_cfg->model_outshape_) {
  //    uint32_t output_sum_temp = std::accumulate(
  //      std::begin(outshape), std::end(outshape), 1,
  //      std::multiplies<uint32_t>());
  //    output_sum *= output_sum_temp;
  //  }
  return true;
}

bool Pipeline::InitPipeline(const std::vector<ModelCfgPtr> &model_cfgs) {
  if (model_cfgs.empty()) {
    LOG(ERROR) << "Pipeline init failed, model cfgs is emtpy!";
    return false;
  }
  stages_.resize(model_cfgs.size());
  for (const auto &model_cfg : model_cfgs) {
    RegisterStage(model_cfg);
  }
  is_ready_.store(true);
  return true;
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
                    stage->RunStage(cur_context_ptr);
                  }
                  return true;
                });

  is_ready_.store(true);
}

bool Pipeline::PushDatatoPipeline(char **input_data,
                                  const std::vector<uint32_t> &input_size) {
  for (size_t i = 0; i < stages_[0].size(); ++i) {
    std::string stage_name = stages_[0][i]->GetModelName();
    auto stage_context_ptr = GetStageContextbyName(stage_name);
    if (stage_context_ptr == nullptr) {
      return false;
    }
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

DeviceStage::DeviceStage(const ModelCfgPtr &model_cfg)
    : AbstractStage(model_cfg) {
  engine_ = device::DeviceEngineFactory::GetInstance().GetDeviceInference(
    model_cfg->model_name_, model_cfg->model_type_);
  engine_->SetModelCfg(model_cfg);
};

bool DeviceStage::FillStagebyEngine(const contextPtr &conext_ptr) {
  if (engine_ == nullptr) {
    LOG(ERROR) << "DeviceStage obtain engine is nullptr";
    return false;
  }
  REPORT_ERROR_IF_NULL(conext_ptr);
  engine_->CreateGraph(conext_ptr);
  return true;
}

bool DeviceStage::RunStage(const contextPtr &conext_ptr) {
  // TODO：add run graph
  engine_->RunProcess(conext_ptr);
  return true;
}

bool DecoratorStage::RunStage(const contextPtr &conext_ptr) {
  StagePreProcess(conext_ptr);
  RunSubStage(conext_ptr);
  StagePostProcess(conext_ptr);
  return true;
}

bool DecoratorStage::AddProcess(const DeviceStagePtr &device_ptr) {
  REPORT_ERROR_IF_NULL(device_ptr);
  extra_stage_ptr_ = device_ptr;
  stage_name_ = device_ptr->GetModelName();
  return true;
}
}  // namespace pipeline