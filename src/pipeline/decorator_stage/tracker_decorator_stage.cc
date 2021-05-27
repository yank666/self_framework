//
// Created by yankai.yan on 2021/5/25.
//

#include "tracker_decorator_stage.h"
#include "glog/logging.h"
#include "src/utils/base.h"
#include "src/pipeline/stage_register.h"
#include "src/utils/common_struct.h"

namespace pipeline {
TrackerDerocatestage::TrackerDerocatestage() {}

TrackerDerocatestage::~TrackerDerocatestage() {
  if (tracker_api != nullptr) {
    tracker_release(tracker_api);
  }
}

bool TrackerDerocatestage::StagePreProcess(
  const contextPtr &conext_ptr, const ProcessContextMap &contextMap) {
  REPORT_ERROR_IF_NULL(conext_ptr);
  TimeWatch time_watch;
  time_watch.start();
  if (tracker_api == nullptr) {
    tracker_api = tracker_init_with_config("config_person_mobile.json");
  }
  LOG(INFO) << "Run Tracker predecorate stage run success, cost"
            << time_watch.stop() << "ms";
  return true;
}

bool TrackerDerocatestage::StagePostProcess(
  const contextPtr &conext_ptr, const ProcessContextMap &contextMap) {
  REPORT_ERROR_IF_NULL(conext_ptr);
  TimeWatch time_watch;
  time_watch.start();
  detect_res body_det;
  detect_res body_track;
  body_det.num_detections = 0;
  body_track.num_detections = 0;
  auto det_iter = contextMap.find("yolo");
  if (det_iter == contextMap.end()) {
    return false;
  }
  auto det_context = det_iter->second.second;
  REPORT_EXCEPTION_IF_NULL(det_context);
  std::vector<PersonInfo> detect_infos;
  detect_infos.resize(det_context->out_dataflow_.size());
  for (auto i = 0; i < detect_infos.size(); i++) {
    memcpy(&(detect_infos[i]), det_context->out_dataflow_[i].data(),
           det_context->out_dataflow_[i].size());
  }
  for (auto idx = 0; idx < detect_infos.size(); idx++) {
    body_det.detect_res_info[body_det.num_detections].track_id = -1;
    body_det.detect_res_info[body_det.num_detections].label = 0;
    body_det.detect_res_info[body_det.num_detections].prob =
      detect_infos[idx].body.conf;
    body_det.detect_res_info[body_det.num_detections].quality =
      1 - detect_infos[idx].attrs[2].second;
    body_det.detect_res_info[body_det.num_detections].box.x1 =
      detect_infos[idx].body.x1;
    body_det.detect_res_info[body_det.num_detections].box.y1 =
      detect_infos[idx].body.y1;
    body_det.detect_res_info[body_det.num_detections].box.x2 =
      detect_infos[idx].body.x2;
    body_det.detect_res_info[body_det.num_detections].box.y2 =
      detect_infos[idx].body.y2;
    body_det.num_detections++;
  }
  cv::Mat img = cv::Mat(conext_ptr->input_h, conext_ptr->input_w, CV_8UC3,
                        conext_ptr->ori_data[0].data(),
                        conext_ptr->input_w * 3 * sizeof(uint8_t));
  cv::Mat clone = img.clone();
  tracker_inference(&body_det, &body_track, 0, clone.data, clone.cols,
                    clone.rows, 3, tracker_api);
  conext_ptr->out_dataflow_.clear();
  conext_ptr->out_dataflow_.resize(1);
  conext_ptr->out_dataflow_[0].resize(sizeof(detect_res));
  memcpy(conext_ptr->out_dataflow_[0].data(), &body_track, sizeof(detect_res));
  LOG(INFO) << "tracker output size is :" << sizeof(detect_res);
  LOG(INFO) << "Run Tracker postdecorate stage run success, cost"
            << time_watch.stop() << "ms";
  return true;
}
REG_Stage(tracker, std::make_shared<TrackerDerocatestage>())
}  // namespace pipeline