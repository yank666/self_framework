//
// Created by yankai on 2021/3/8.
//

#ifndef SELF_ARCHITECTURE_REID_DECORATOR_STAGE_H
#define SELF_ARCHITECTURE_REID_DECORATOR_STAGE_H

#include "src/pipeline/pipeline.h"
namespace pipeline {
struct Box {
  float x1;
  float y1;
  float x2;
  float y2;
  float conf;
};

struct BodyInfo {
  float score;
  int type;
  Box box;
  Box related_box;
  float related_score;
  std::vector<std::pair<int, float>> attrs;  // 属性label: 属性score
};

struct Person {
  int32_t label;
  Box body;
  Box head;
  Box face;

  std::vector<Box> hand_packages;
  std::vector<std::pair<int, float>> attrs;
  std::vector<float> feature;
  bool face_observeed = false;
  bool handpackage_observed = false;
};

class ReidDerocatestage : public DecoratorStage {
  bool StagePreProcess(const contextPtr &conext_ptr,
                       const ProcessContextMap &contextMap);
  bool StagePostProcess(const contextPtr &conext_ptr,
                        const ProcessContextMap &contextMap);
};
}  // namespace pipeline

#endif  // SELF_ARCHITECTURE_REID_DECORATOR_STAGE_H
