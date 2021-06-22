//
// Created by yankai.yan on 2021/5/17.
//

#include "upload_decorator_stage.h"
#include <algorithm>
#include "glog/logging.h"
#include "src/pipeline/stage_register.h"
#include "src/utils/common_struct.h"
#include "src/utils/base.h"
#include "base64encoding/base64.h"

namespace pipeline {
constexpr bool kNeedDump = true;
const std::string kStreamInfo =
  "{\"streamId\":\"1022\",\"equipmentId\":\"1022\",\"stream_width\":1280,"
  "\"stream_height\":720,\"taskType\":\"realVideoStream\",\"config\":{"
  "\"Graphs\":[{\"areaId\":\"1\",\"shapeType\":\"checkOut\",\"shapeList\":[{"
  "\"x\":\"0.0\",\"y\":\"0.0\"},{\"x\":\"0.0\",\"y\":\"1.0\"},{\"x\":\"1.0\","
  "\"y\":\"1.0\"},{\"x\":\"1.0\",\"y\":\"0.0\"}],\"algoType\":"
  "\"regionAttribute\"}]}}";

inline bool PointInPolygon(const cv::Point2f &pt,
                           const std::vector<cv::Point2d> &vertices) {
  size_t nvert = vertices.size();
  size_t i, j = 0;
  bool inside = false;
  for (i = 0, j = nvert - 1; i < nvert; j = i++) {
    if (((vertices[i].y >= pt.y) != (vertices[j].y >= pt.y)) &&
        (pt.x <= (vertices[j].x - vertices[i].x) * (pt.y - vertices[i].y) /
                     (vertices[j].y - vertices[i].y) +
                   vertices[i].x)) {
      inside = !inside;
    }
  }
  return inside;
}

std::string UploadDerocatestage::MatBase64Encode(const cv::Mat &src) {
  std::vector<uchar> data_encode;
  cv::imencode(".jpg", src, data_encode);
  return base64_encode(data_encode.data(), data_encode.size());
}

inline std::string UploadDerocatestage::AddValuetoJson(
  const std::string &val, const std::string &new_val) {
  return val + new_val + ";";
}

bool UploadDerocatestage::StagePreProcess(const contextPtr &conext_ptr,
                                          const ProcessContextMap &contextMap) {
  REPORT_EXCEPTION_IF_NULL(conext_ptr);
  TimeWatch time_watch;
  time_watch.start();
  if (stream_info_ == nullptr) {
    stream_info_ = std::make_shared<StreamInfo>();
  }
  if (ParseJson::ParseVideoJson(const_cast<char *>(kStreamInfo.data()),
                                stream_info_)) {
    return false;
  }
  DLOG(INFO) << "Run Upload PreDecorate stage run success, cost"
             << time_watch.stop() << "ms";
  return true;
}

bool UploadDerocatestage::StagePostProcess(
  const contextPtr &conext_ptr, const ProcessContextMap &contextMap) {
  REPORT_EXCEPTION_IF_NULL(conext_ptr);
  TimeWatch time_watch;
  time_watch.start();

  detect_res obj;
  memcpy(&obj, conext_ptr->dataflow_[0].data(),
         conext_ptr->dataflow_[0].size());

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

  for (uint32_t idx = 0; idx < obj.num_detections; ++idx) {
    json json_val;
    uint32_t cur_id = obj.detect_res_info[idx].track_id;
    if (cur_id == -1) {
      continue;
    }
    UpdataUpLoadJson(conext_ptr, obj.detect_res_info[idx], detect_infos[idx]);
    SelectAttrofUploadJson(conext_ptr, obj.detect_res_info[idx],
                           detect_infos[idx]);
  }

  for (auto &json_item : upload_struct_) {
    std::ofstream file(std::to_string(json_item.first) + "key.json");
    file << std::setw(4) << json_item.second << std::endl;
    file.close();
  }
  DLOG(INFO) << "Run Upload PostDecorate stage run success, cost"
             << time_watch.stop() << "ms";
  return true;
}

bool UploadDerocatestage::UpdataUpLoadJson(const contextPtr &conext_ptr,
                                           const detect_bbox_info &detect,
                                           const PersonInfo &person) {
  json json_val;
  uint32_t body_count = 0, handpk_count = 0;
  std::string body_time, body_ltx, body_lty, body_rbx, body_rby;
  std::string handpk_time, handpk_ltx, handpk_lty, handpk_rbx, handpk_rby;
  float cur_body_prob = 0.0f, cur_face_prob = 0.0f;
  long endtime_stamp;
  uint32_t cur_id = detect.track_id;
  cv::Mat src = cv::Mat(conext_ptr->input_h, conext_ptr->input_w, CV_8UC3,
                        conext_ptr->ori_data[0].data());
  if (upload_struct_.count(cur_id) != 0) {
    json_val = upload_struct_[cur_id];
    body_count = json_val["bodyInfo"]["bodyTrailNum"];
    body_time = json_val["bodyInfo"]["bodyTrailPointTime"];
    body_ltx = json_val["bodyInfo"]["bodyTrailLeftTopX"];
    body_lty = json_val["bodyInfo"]["bodyTrailLeftTopY"];
    body_rbx = json_val["bodyInfo"]["bodyTrailRightBtmX"];
    body_rby = json_val["bodyInfo"]["bodyTrailRightBtmY"];
    cur_body_prob = json_val["bodyInfo"]["bodyProb"];

    cur_face_prob = json_val["faceInfo"]["faceProb"];

    handpk_count = json_val["handpackageInfo"]["handpackageTrailNum"];
    handpk_time = json_val["handpackageInfo"]["handpackageTrailPointTime"];
    handpk_ltx = json_val["handpackageInfo"]["handpackageTrailLeftTopX"];
    handpk_lty = json_val["handpackageInfo"]["handpackageTrailLeftTopY"];
    handpk_rbx = json_val["handpackageInfo"]["handpackageTrailRightBtmX"];
    handpk_rby = json_val["handpackageInfo"]["handpackageTrailRightBtmY"];
  } else {
    json_val["statusCode"] = status_Success;
    json_val["streamId"] = conext_ptr->stream_id_;
    json_val["equipmentId"] = conext_ptr->equipment_id_;
    json_val["id"] = cur_id;
    json_val["bodyIsObserved"] = true;
    json_val["startTimestamp"] = conext_ptr->time_stamp_;
    json_val["bodyIsObserved"] = true;
    json_val["handpackageInfo"]["handpackageTrailNum"] = 0;
    json_val["handpackageInfo"]["handpackageTrailPointTime"] = "";
    json_val["handpackageInfo"]["handpackageTrailLeftTopX"] = "";
    json_val["handpackageInfo"]["handpackageTrailLeftTopY"] = "";
    json_val["handpackageInfo"]["handpackageTrailRightBtmX"] = "";
    json_val["handpackageInfo"]["handpackageTrailRightBtmY"] = "";
    json_val["bodyInfo"]["bodyProb"] = 0.0;
    json_val["faceInfo"]["faceProb"] = 0.0;
  }
  json_val["endTimestamp"] = conext_ptr->time_stamp_;
  json_val["bodyInfo"]["bodyTrailNum"] = ++body_count;
  json_val["bodyInfo"]["bodyTrailPointTime"] =
    AddValuetoJson(body_time, std::to_string(conext_ptr->time_stamp_));
  json_val["bodyInfo"]["bodyTrailLeftTopX"] =
    AddValuetoJson(body_ltx, std::to_string(detect.box.x1));
  json_val["bodyInfo"]["bodyTrailLeftTopY"] =
    AddValuetoJson(body_lty, std::to_string(detect.box.y1));
  json_val["bodyInfo"]["bodyTrailRightBtmX"] =
    AddValuetoJson(body_rbx, std::to_string(detect.box.x2));
  json_val["bodyInfo"]["bodyTrailRightBtmY"] =
    AddValuetoJson(body_rby, std::to_string(detect.box.y2));
  json_val["bodyInfo"]["bodyQuality"] = detect.quality;
  {  // no use ,but need to set
    json_val["bodyInfo"]["bodyAttributes"] = "";
    json_val["bodyInfo"]["faceKeypointQuality"] = "";
    json_val["bodyInfo"]["feat"] = "";
    json_val["bodyInfo"]["faceQuality"] = "";
  }
  if (detect.prob > cur_body_prob) {
    json_val["bodyInfo"]["bodyProb"] = detect.prob;
    json_val["bodyInfo"]["selectedTimestamp"] = conext_ptr->time_stamp_;
    json_val["bodyInfo"]["feat"] = detect.feature;
    json_val["bodyInfo"]["box"] = {person.body.x1, person.body.y1,
                                   person.body.x2, person.body.y2};

    cv::Mat roi =
      src(cv::Rect(detect.box.x1, detect.box.y1, detect.box.x2 - detect.box.x1,
                   detect.box.y2 - detect.box.y1))
        .clone();
    json_val["bodyInfo"]["croppedPicBase64"] = MatBase64Encode(roi);
  }
  if (person.face_observeed) {
    json_val["faceIsObserved"] = true;
    if (person.face_observeed && person.face.conf > cur_face_prob) {
      cv::Mat face = src(cv::Rect(person.face.x1, person.face.y1,
                                  person.face.x2 - person.face.x1,
                                  person.face.y2 - person.face.y1))
                       .clone();
      json_val["faceInfo"]["croppedPicBase64"] = MatBase64Encode(face);
      json_val["faceInfo"]["box"] = {person.face.x1, person.face.y1,
                                     person.face.x2, person.face.y2};
      json_val["faceInfo"]["selectedTimestamp"] = conext_ptr->time_stamp_;
      json_val["faceInfo"]["faceProb"] = person.face.conf;
    }
  }

  for (int cnt = 0; cnt < person.hand_packages_nums; cnt++) {
    json_val["handpackageInfo"]["handpackageTrailNum"] = ++handpk_count;
    json_val["handpackageInfo"]["handpackageTrailPointTime"] =
      AddValuetoJson(handpk_time, std::to_string(conext_ptr->time_stamp_));
    json_val["handpackageInfo"]["handpackageTrailLeftTopX"] =
      AddValuetoJson(handpk_ltx, std::to_string(person.hand_packages[cnt].x1));
    json_val["handpackageInfo"]["handpackageTrailLeftTopY"] =
      AddValuetoJson(handpk_lty, std::to_string(person.hand_packages[cnt].y1));
    json_val["handpackageInfo"]["handpackageTrailRightBtmX"] =
      AddValuetoJson(handpk_rbx, std::to_string(person.hand_packages[cnt].x2));
    json_val["handpackageInfo"]["handpackageTrailRightBtmY"] =
      AddValuetoJson(handpk_rby, std::to_string(person.hand_packages[cnt].y2));
  }

  upload_struct_[cur_id] = json_val;
  return true;
}

void UploadDerocatestage::SelectAttrofUploadJson(const contextPtr &conext_ptr,
                                                 const detect_bbox_info &detect,
                                                 const PersonInfo &person) {
  uint32_t cur_id = detect.track_id;
  auto json_val = upload_struct_[cur_id];

  std::vector<cv::Point2f> bbox;
  for (int regionIdx = 0; regionIdx < stream_info_->config.size();
       regionIdx++) {
    bool is_in_poly = false;
    for (int j = 0; j < bbox.size(); j++) {
      is_in_poly =
        PointInPolygon(bbox[j], stream_info_->config[regionIdx].polygon) ||
        is_in_poly;
    }
    if (is_in_poly) {
      std::string shape_type = stream_info_->config[regionIdx].shapeType;
      if (!json_val["attributes"].contains(shape_type)) {
        json_val["attributes"][shape_type] = 1;
      } else {
        uint32_t shape_count = json_val["attributes"][shape_type];
        shape_count += 1;
        json_val["attributes"][shape_type] = shape_count;
      }
      json_val["regionSnapshots"][shape_type]["imgae"] =
        json_val["bodyInfo"]["croppedPicBase64"];
      if (json_val["regionSnapshots"][shape_type]["startTimestamp"].empty()) {
        json_val["regionSnapshots"][shape_type]["startTimestamp"] =
          conext_ptr->time_stamp_;
      }
      json_val["regionSnapshots"][shape_type]["endTimestamp"] =
        conext_ptr->time_stamp_;
    }
  }
  upload_struct_[cur_id] = json_val;
}

REG_Stage(upload, std::make_shared<UploadDerocatestage>())
}  // namespace pipeline