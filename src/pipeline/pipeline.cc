
#include "pipeline.h"
#include <iostream>
#include <algorithm>
#include <numeric>
//#include "glog/logging.h"
#include "register_stage.h"

namespace pipeline{
enum ContextIdx: uint32_t {datainput = 0, dataoutput = 1, dataidx = 2};

void Pipeline::RegisterProcess(const ModelCfgPtr &model_cfg) {
  DeviceStagePtr model_inference_ptr = std::make_shared<DeviceStage>(model_cfg);
  if (model_inference_ptr == nullptr) {
    //    LOG(ERROR) << "Pipeline init failed, model cfgs is emtpy!";
    return;
  }
  model_inference_ptr->CreateContextFromCfg();
  
  DecorStagePtr exec_stage_ptr =
      DeviceInferenceFactory::GetInstance().GetDeviceInference(model_inference_ptr->GetModelName());
  if (exec_stage_ptr != nullptr) {
    exec_stage_ptr->AddProcess(model_inference_ptr);
    stages_[model_cfg->model_position_].push_back(exec_stage_ptr);
  } else {
    stages_[model_cfg->model_position_].push_back(model_inference_ptr);
  }
}

bool Pipeline::SetDataFlowBuf(const std::vector<AbstractStagePtr> &cur_stage_vec) {
  for (auto &cur_stage : cur_stage_vec) {
    contextPtr cut_stage_ctx = cur_stage->GetContext();
    if (cut_stage_ctx == nullptr) {
      // LOG(ERROR) << "First stage in pipeline obtain context return nullptr!
      return false;
    }
    cut_stage_ctx->dataflow_.resize(dataidx);
    auto stage_input_size = cut_stage_ctx->datasize_[datainput];
    cut_stage_ctx->dataflow_[datainput].resize(stage_input_size);
    auto stage_output_size = cut_stage_ctx->datasize_[dataoutput];
    cut_stage_ctx->dataflow_[dataoutput].resize(stage_output_size);
  }
}

bool Pipeline::InitPipeline(const std::vector<ModelCfgPtr> &model_cfgs,
                                  char **input_data) {
  if (model_cfgs.empty()) {
//    LOG(ERROR) << "Pipeline init failed, model cfgs is emtpy!";
    return false;
  }
  stages_.resize(model_cfgs.size());
  for (auto iter = model_cfgs.begin(); iter != model_cfgs.end(); ++iter) {
    RegisterProcess(*iter);
  }
  for (auto iter = stages_.begin(); iter != stages_.end(); ++iter) {
    SetDataFlowBuf(*iter);
  }

  for (size_t i = 0; i < stages_[0].size(); ++i) {
    contextPtr cut_stage_ctx = stages_[0][i]->GetContext();
    auto stage_data_input = cut_stage_ctx->dataflow_[datainput];
//    memcpy(stage_data_input.data(), input_data[i],
//        cut_stage_ctx->datasize_[datainput] * sizeof(float));
  }
}

void Pipeline::RunPipeline() {
  char *delivery_ptr = nullptr;
  std::for_each(stages_.begin(), stages_.end(),
      [&](const std::vector<AbstractStagePtr> & stage_vec) {
    // TODO (yankai): consider add multithread exec staege ,if stage_vec size is more than 1;
    for (auto &stage : stage_vec) {
      char *input_array = (delivery_ptr != nullptr)? delivery_ptr :
          reinterpret_cast<char *>(stage->GetContext()->dataflow_[datainput].data());
      stage->RunStage(delivery_ptr);
//      delivery_ptr = reinterpret_cast<char *>(
//          stage->GetContext()->dataflow_[dataoutput].data());
    }
  });
}

bool AbstractStage::CreateContextFromCfg() {
  if(stage_cfg_ == nullptr) {
    // LOG(ERROR) << "Create context failed , because input cfg is nullptr!";
    return false;
  }
  stage_context_ = std::make_shared<Context>();
  if (stage_context_ == nullptr) {
    // LOG(ERROR) << "Create context failed , create model context failed!";
    return true;
  }
  stage_context_->name_ = stage_cfg_->model_name_;
  stage_context_->datasize_.resize(dataidx);
  uint32_t  input_sum = 1;
  for (auto &inshape : stage_cfg_->model_inshape_) {
    uint32_t input_sum_temp = std::accumulate(std::begin(inshape), std::end(inshape),
        1, std::multiplies<int>());
    input_sum *= input_sum_temp;
  }
  stage_context_->datasize_[datainput] = input_sum;
  uint32_t  output_sum = 1;
  for (auto &outshape : stage_cfg_->model_outshape_) {
    uint32_t output_sum_temp = std::accumulate(std::begin(outshape), std::end(outshape),
        1, std::multiplies<int>());
    output_sum *= output_sum_temp;
  }
  stage_context_->datasize_[dataoutput] = output_sum;
}

bool DeviceStage::RunStage(char *input_array) {

  // TODO：add run graph
//  char *output_array = RunGraph();
//  memcpy(stage_context_->dataflow_[dataoutput].data(), output_array,
//      stage_context_->datasize_[dataoutput] * sizeof(float));
  std::cout << " Run DeviceStage" << stage_name_ <<std::endl;
}

bool DecoratorStage::RunStage(char *input_array) {
  StagePreProcess(input_array);
  RunSubStage(input_array);
  StagePostProcess();
}

bool DecoratorStage::StagePreProcess(char *input_array) {

}

bool DecoratorStage::StagePostProcess() {

}

bool DecoratorStage::AddProcess(const DeviceStagePtr &device_ptr) {
  if (device_ptr == nullptr) {
    //LOG(ERROR) << "DecoratorStage add process fail, becaus devicePte is nullptr";
    return false;
  }
  extra_stage_ptr_ = device_ptr;
  stage_cfg_ = device_ptr->GetModelCfg();
  stage_context_ = device_ptr->GetContext();
  return true;
};
}