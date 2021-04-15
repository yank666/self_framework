//
// Created by yankai.yan on 2021/4/15.
//

#ifndef SELF_ARCHITECTURE_YOLO_DECORATOR_STAGE_H
#define SELF_ARCHITECTURE_YOLO_DECORATOR_STAGE_H

#include "src/pipeline/pipeline.h"
namespace pipeline {
class YoloDerocatestage : public DecoratorStage {
  bool StagePreProcess(const contextPtr &conext_ptr);
  bool StagePostProcess(const contextPtr &conext_ptr);
};
}

#endif // SELF_ARCHITECTURE_YOLO_DECORATOR_STAGE_H
