//
// Created by yankai on 2021/3/8.
//

#ifndef SELF_ARCHITECTURE_FCOS_DEROCATE_STAGE_H
#define SELF_ARCHITECTURE_FCOS_DEROCATE_STAGE_H

#include "src/pipeline/pipeline.h"
namespace pipeline {
class FcosDerocatestage : public DecoratorStage {
 protected:
  bool StagePreProcess(const contextPtr &conext_ptr,
                       const ProcessContextMap &contextMap);
  bool StagePostProcess(const contextPtr &conext_ptr,
                        const ProcessContextMap &contextMap);
};
}  // namespace pipeline

#endif  // SELF_ARCHITECTURE_FCOS_DEROCATE_STAGE_H
