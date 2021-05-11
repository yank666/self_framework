#include "reid_decorator_stage.h"
#include "src/pipeline/stage_register.h"
#include <iostream>
#include "glog/logging.h"

namespace pipeline {
bool ReidDerocatestage::StagePreProcess(const contextPtr &conext_ptr) {
  LOG(INFO) << " Run ReidDerocatestage pre new";
  Person person12;
  std::memcpy(&person12, conext_ptr->dataflow_[0].data(), sizeof(Person));

  cv::Mat img = cv::Mat(conext_ptr->input_h, conext_ptr->input_w, CV_8UC3,
                        conext_ptr->ori_data[0].data());

  cv::Mat roi = img(cv::Rect(person12.body.x1, person12.body.y1,
                             person12.body.x2 - person12.body.x1,
                             person12.body.y2 - person12.body.y1));
  cv::Mat resize_roi;
  cv::resize(roi, resize_roi, cv::Size(128, 384));

  conext_ptr->dataflow_.clear();
  conext_ptr->dataflow_.resize(1);
  conext_ptr->dataflow_[0].resize(128 * 384 * 3 * sizeof(uchar));
  std::memcpy(conext_ptr->dataflow_[0].data(), resize_roi.data,
              128 * 384 * 3 * sizeof(uchar));
  LOG(INFO) << " Run ReidDerocatestage pre 5";
  return true;
}

bool ReidDerocatestage::StagePostProcess(const contextPtr &conext_ptr) {
  LOG(INFO) << " Run ReidDerocatestage post";

  return true;
}

REG_Stage(reid, std::make_shared<ReidDerocatestage>())
}  // namespace pipeline