//
// Created by yankai.yan on 2021/4/15.
//

#include "yolo_decorator_stage.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "glog/logging.h"
#include "src/pipeline/stage_register.h"

namespace pipeline {
bool YoloDerocatestage::StagePreProcess(const contextPtr &conext_ptr) {
  LOG(INFO) << "Run Yolo predecorate stage success";
  return true;
}

bool YoloDerocatestage::StagePostProcess(const contextPtr &conext_ptr) {


  LOG(INFO) << "Run Yolo postdecorate stage success";
  return true;

}
REG_Stage(yolo, std::make_shared<YoloDerocatestage>())
}