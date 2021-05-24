#include "reid_decorator_stage.h"
#include "src/pipeline/stage_register.h"
#include <iostream>
#include "glog/logging.h"
#include "src/utils/base.h"

namespace pipeline {
bool ReidDerocatestage::StagePreProcess(const contextPtr &conext_ptr,
                                        const ProcessContextMap &contextMap) {
  REPORT_ERROR_IF_NULL(conext_ptr);
  TimeWatch time_watch;
  time_watch.start();
  std::vector<Person> person_vec;
  LOG(INFO) << "Run Reid predecorate batches " <<  conext_ptr->batches;
  for (auto i = 0; i < conext_ptr->dataflow_.size();i++) {
    Person person_obj;
    std::memcpy(&person_obj, conext_ptr->dataflow_[i].data(), sizeof(Person));

    cv::Mat img = cv::Mat(conext_ptr->input_h, conext_ptr->input_w, CV_8UC3,
                          conext_ptr->ori_data[0].data());

    cv::Mat roi = img(cv::Rect(person_obj.body.x1, person_obj.body.y1,
                               person_obj.body.x2 - person_obj.body.x1,
                               person_obj.body.y2 - person_obj.body.y1));
    cv::Mat resize_roi;
    cv::resize(roi, resize_roi, cv::Size(128, 384));
    cv::imwrite(std::to_string(i) + "_roi.jpg", resize_roi);
    conext_ptr->dataflow_[i].clear();
    conext_ptr->dataflow_[i].resize(128 * 384 * 3 * sizeof(uchar));
    std::memcpy(conext_ptr->dataflow_[i].data(), resize_roi.data,
                128 * 384 * 3 * sizeof(uchar));
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
  auto outputs_vecs = conext_ptr->out_dataflow_;
  LOG(INFO) << "Run Reid postdecorate size " <<  outputs_vecs.size();
  for (auto i = 0; i < outputs_vecs.size(); i++) {
    LOG(INFO) << " reid outputs_vecs size  is " << outputs_vecs[i].size();
  }
  LOG(INFO) << "Run Reid postdecorate stage run success, cost"
            << time_watch.stop() << "ms";
  return true;
}

REG_Stage(reid, std::make_shared<ReidDerocatestage>())
}  // namespace pipeline