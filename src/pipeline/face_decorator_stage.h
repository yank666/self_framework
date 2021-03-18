//
// Created by yankai on 2021/3/8.
//

#ifndef SELF_ARCHITECTURE_FACE_DECORATOR_STAGE_H
#define SELF_ARCHITECTURE_FACE_DECORATOR_STAGE_H

#include "pipeline.h"
namespace pipeline {
class FaceDerocatestage : public DecoratorStage {
  bool StagePreProcess(const ProcessContextMap &conext_map);
  bool StagePostProcess(const ProcessContextMap &conext_map);
};
}


#endif //SELF_ARCHITECTURE_FACE_DECORATOR_STAGE_H
