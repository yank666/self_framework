//
// Created by yankai.yan on 2021/5/25.
//

#ifndef SELF_ARCHITECTURE_TRACKER_DECORATOR_STAGE_H
#define SELF_ARCHITECTURE_TRACKER_DECORATOR_STAGE_H

#include "src/pipeline/pipeline.h"
#include "thirdparty/tracker/TrackerLib_api_c.h"
namespace pipeline {
class TrackerDerocatestage : public DecoratorStage {
 public:
  TrackerDerocatestage();
  ~TrackerDerocatestage();

 protected:
  bool StagePreProcess(const contextPtr &conext_ptr,
                       const ProcessContextMap &contextMap);
  bool StagePostProcess(const contextPtr &conext_ptr,
                        const ProcessContextMap &contextMap);

 private:
   uint32_t frame_idx_;
  void *tracker_api{nullptr};
};
}  // namespace pipeline

#endif  // SELF_ARCHITECTURE_TRACKER_DECORATOR_STAGE_H
