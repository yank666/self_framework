//
// Created by yankai on 2021/3/8.
//

#ifndef SELF_ARCHITECTURE_FACE_DECORATOR_STAGE_H
#define SELF_ARCHITECTURE_FACE_DECORATOR_STAGE_H

#include "pipeline.h"
namespace pipeline {
class FaceDerocatestage : public DecoratorStage {
  bool StagePreProcess(char *input_array);
  bool StagePostProcess();
};
}


#endif //SELF_ARCHITECTURE_FACE_DECORATOR_STAGE_H
