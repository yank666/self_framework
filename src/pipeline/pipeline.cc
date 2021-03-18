#include "pipeline.h"
#include <iostream>
#include <algorithm>
#include <numeric>
#include "glog/logging.h"
#include "register_stage.h"

namespace pipeline{
enum ContextIdx: uint32_t {datainput = 0, dataoutput = 1, dataidx = 2};

void Pipeline::RegisterStage(const ModelCfgPtr &model_cfg) {
  DeviceStagePtr model_inference_ptr = std::make_shared<DeviceStage>(model_cfg);
  if (model_inference_ptr == nullptr) {
    LOG(ERROR) << "Pipeline init failed, model cfgs is emtpy!";
    return;
  }
  contextPtr stage_context = std::make_shared<Context>();
  CreateContexts(stage_context, model_cfg);

  DecorStagePtr exec_stage_ptr =
      DeviceInferenceFactory::GetInstance().GetDeviceInference(model_inference_ptr->GetModelName());
  if (exec_stage_ptr != nullptr) {
    exec_stage_ptr->AddProcess(model_inference_ptr);
    stages_[model_cfg->model_position_].push_back(exec_stage_ptr);
    process_context_.insert(std::make_pair(
        model_cfg->model_name_,
                                           std::make_pair(exec_stage_ptr, stage_context)));
  } else {
    stages_[model_cfg->model_position_].push_back(model_inference_ptr);
    process_context_.insert(std::make_pair(model_cfg->model_name_,
                                           std::make_pair(model_inference_ptr, stage_context)));
  }
}

bool Pipeline::CreateContexts(const contextPtr &cur_context_ptr, const ModelCfgPtr &cur_model_cfg) {
  if (cur_context_ptr == nullptr || cur_model_cfg == nullptr) {
    // LOG(ERROR) << "Create context failed , create model context failed!";
    return true;
  }
  cur_context_ptr->name_ = cur_model_cfg->model_name_;
  cur_context_ptr->datasize_.resize(dataidx);
  cur_context_ptr->dataflow_.resize(dataidx);
  uint32_t  input_sum = 1;
  for (auto &inshape : cur_model_cfg->model_inshape_) {
    uint32_t input_sum_temp = std::accumulate(std::begin(inshape),
                std::end(inshape), 1, std::multiplies<uint32_t>());
    input_sum *= input_sum_temp;
  }
  cur_context_ptr->datasize_[datainput] = input_sum;
  cur_context_ptr->dataflow_[datainput].resize(input_sum);
  uint32_t  output_sum = 1;
  for (auto &outshape : cur_model_cfg->model_outshape_) {
    uint32_t output_sum_temp = std::accumulate(std::begin(outshape),
                std::end(outshape),1, std::multiplies<uint32_t>());
    output_sum *= output_sum_temp;
  }
  cur_context_ptr->datasize_[dataoutput] = output_sum;
  cur_context_ptr->dataflow_[dataoutput].resize(output_sum);
}

bool Pipeline::InitPipeline(const std::vector<ModelCfgPtr> &model_cfgs,
                                  char **input_data) {
  if (model_cfgs.empty()) {
//    LOG(ERROR) << "Pipeline init failed, model cfgs is emtpy!";
    return false;
  }
  stages_.resize(model_cfgs.size());
  for (const auto & model_cfg : model_cfgs) {
    RegisterStage(model_cfg);
  }

  for (size_t i = 0; i < stages_[0].size(); ++i) {
    std::string stage_name= stages_[0][i]->GetModelName();
    auto stage_context_ptr  = GetStageContextbyName(stage_name);
//    auto stage_data_input = stage_context.second->dataflow_[datainput];
//    memcpy(stage_data_input.data(), input_data[i],
//        cut_stage_ctx->datasize_[datainput] * sizeof(float));
  }
}

void Pipeline::RunPipeline() {
  std::for_each(stages_.begin(), stages_.end(),
      [&](const std::vector<AbstractStagePtr> & stage_vec) {
    // TODO (yankai): consider add multithread exec staege ,if stage_vec size is more than 1;
    for (auto &stage : stage_vec) {
      stage->RunStage(process_context_);
    }
  });
}

contextPtr Pipeline::GetStageContextbyName(const std::string &stage_name){
  auto iter = process_context_.find(stage_name);
  if (iter != process_context_.end()) {
    const ProcessContext &process_ctx = iter->second;
    return process_ctx.second;
  }
  return nullptr;
}

bool DeviceStage::RunStage(const ProcessContextMap &conext_map) {
  // TODO：add run graph
  std::cout << " Run DeviceStage" << stage_name_ <<std::endl;
}

bool DecoratorStage::RunStage(const ProcessContextMap &conext_map) {
  StagePreProcess(conext_map);
  RunSubStage(conext_map);
  StagePostProcess(conext_map);
}

bool DecoratorStage::AddProcess(const DeviceStagePtr &device_ptr) {
  if (device_ptr == nullptr) {
    //LOG(ERROR) << "DecoratorStage add process fail, becaus devicePte is nullptr";
    return false;
  }
  extra_stage_ptr_ = device_ptr;
  stage_name_ = device_ptr->GetModelName();
  return true;
}
}