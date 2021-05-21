//
// Created by yankai.yan on 2021/5/17.
//

#ifndef SELF_ARCHITECTURE_UPLOAD_DECORATOR_STAGE_H
#define SELF_ARCHITECTURE_UPLOAD_DECORATOR_STAGE_H

#include "src/pipeline/pipeline.h"
#include "json/json.hpp"
#include "src/parse/parse_json.h"
#include "pipeline/decorator_stage/yolo_decorator_stage.h"
#include "tracker/TrackerLib_api_c.h"
namespace pipeline {
using json = nlohmann::ordered_json;

class UploadDerocatestage : public DecoratorStage {
  bool StagePreProcess(const contextPtr &conext_ptr,
                       const ProcessContextMap &contextMap);
  bool StagePostProcess(const contextPtr &conext_ptr,
                        const ProcessContextMap &contextMap);

 protected:
  bool InitUpLoadJson(const contextPtr &conext_ptr,
                      const detect_bbox_info &detect, const PersonInfo &person);
  bool UpdataUpLoadJson(const contextPtr &conext_ptr,
                      const detect_bbox_info &detect, const PersonInfo &person);
  bool SelectAttrofUploadJson(const contextPtr &conext_ptr,
                      const detect_bbox_info &detect, const PersonInfo &person);
//  json json_val_;
  std::shared_ptr<StreamInfo> stream_info_;
  std::unordered_map<int, json> upload_struct_;
};
}  // namespace pipeline

#endif  // SELF_ARCHITECTURE_UPLOAD_DECORATOR_STAGE_H
