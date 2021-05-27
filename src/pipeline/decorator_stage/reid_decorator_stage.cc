#include "reid_decorator_stage.h"
#include <iostream>
#include "glog/logging.h"
#include "src/utils/base.h"
#include "src/pipeline/stage_register.h"
#include "src/utils/common_struct.h"

namespace pipeline {
constexpr uint32_t kReidWidth = 128;
constexpr uint32_t kReidHeight = 384;
constexpr uint32_t kReidChannel = 3;
bool ReidDerocatestage::StagePreProcess(const contextPtr &conext_ptr,
                                        const ProcessContextMap &contextMap) {
  REPORT_ERROR_IF_NULL(conext_ptr);
  TimeWatch time_watch;
  time_watch.start();
  std::vector<PersonInfo> person_vec;
  for (auto i = 0; i < conext_ptr->dataflow_.size(); i++) {
    PersonInfo person_obj;
    std::memcpy(&person_obj, conext_ptr->dataflow_[i].data(),
                sizeof(PersonInfo));
    cv::Mat img = cv::Mat(conext_ptr->input_h, conext_ptr->input_w, CV_8UC3,
                          conext_ptr->ori_data[0].data());
    cv::Mat roi = img(cv::Rect(person_obj.body.x1, person_obj.body.y1,
                               person_obj.body.x2 - person_obj.body.x1,
                               person_obj.body.y2 - person_obj.body.y1));

    cv::Mat resize_roi;
    cv::resize(roi, resize_roi, cv::Size(128, 384));
    conext_ptr->dataflow_[i].clear();
    conext_ptr->dataflow_[i].resize(kReidWidth * kReidHeight * kReidChannel *
                                    sizeof(uchar));
    std::memcpy(conext_ptr->dataflow_[i].data(), resize_roi.data,
                kReidWidth * kReidHeight * kReidChannel * sizeof(uchar));
  }
  LOG(INFO) << "Run Reid predecorate stage run success, cost"
            << time_watch.stop() << "ms";
  return true;
}

bool ReidDerocatestage::StagePostProcess(const contextPtr &conext_ptr,
                                         const ProcessContextMap &contextMap) {
  REPORT_ERROR_IF_NULL(conext_ptr);
  TimeWatch time_watch;
  time_watch.start();
  LOG(INFO) << "Run Reid postdecorate stage run success, cost"
            << time_watch.stop() << "ms";
  return true;
}

REG_Stage(reid, std::make_shared<ReidDerocatestage>())
}  // namespace pipeline