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
const std::string kStreamInfo = "{\"streamId\":\"1022\",\"equipmentId\":\"1022\",\"stream_width\":1280,\"stream_height\":720,\"taskType\":\"realVideoStream\",\"config\":{\"Graphs\":[{\"areaId\":\"1\",\"shapeType\":\"checkOut\",\"shapeList\":[{\"x\":\"0.0\",\"y\":\"0.0\"},{\"x\":\"0.0\",\"y\":\"1.0\"},{\"x\":\"1.0\",\"y\":\"1.0\"},{\"x\":\"1.0\",\"y\":\"0.0\"}],\"algoType\":\"regionAttribute\"}]}}";

inline bool PointInPolygon(const cv::Point2f &pt,
                           const std::vector<cv::Point2d> &vertices){
  size_t nvert = vertices.size();
  size_t i, j = 0;
  bool inside = false;
  for (i = 0, j = nvert - 1; i < nvert; j = i++) {
    if (((vertices[i].y >= pt.y) != (vertices[j].y >= pt.y)) &&
        (pt.x <= (vertices[j].x - vertices[i].x) * (pt.y - vertices[i].y) / (vertices[j].y - vertices[i].y) + vertices[i].x)){
      inside = !inside;
    }
  }
  return inside;
}

bool UploadDerocatestage::StagePreProcess(const contextPtr &conext_ptr,
                                          const ProcessContextMap &contextMap) {
  REPORT_EXCEPTION_IF_NULL(conext_ptr);
  if (stream_info_ == nullptr) {
    stream_info_ = std::make_shared<StreamInfo>();
  }
  if(ParseJson::ParseVideoJson(
        const_cast<char *>(kStreamInfo.data()),stream_info_)) {
    return false;
  }

//      conext_ptr->callback_function_(json_str, strlen(json_str) + 1,
//                                     conext_ptr->stream_id_.c_str(), conext_ptr->stream_id_.size());
  return true;
}

bool UploadDerocatestage::StagePostProcess(
  const contextPtr &conext_ptr, const ProcessContextMap &contextMap) {
  auto det_iter = contextMap.find("yolo");
  if (det_iter == contextMap.end()) {
    return false;
  }
  auto det_context = det_iter->second.second;
  REPORT_EXCEPTION_IF_NULL(det_context);
  std::vector<PersonInfo> detect_infos;
  detect_infos.resize(det_context->out_dataflow_.size());
  for(auto i = 0; i <= detect_infos.size(); i++){
    memcpy(&(detect_infos[i]), det_context->out_dataflow_[i].data(),
           det_context->out_dataflow_[i].size());
  }
  detect_res obj;
  memcpy(&obj, conext_ptr->out_dataflow_[0].data(),conext_ptr->out_dataflow_[0].size());

  for (uint32_t idx = 0; idx < obj.num_detections; ++idx) {
    uint32_t cur_id = obj.detect_res_info[idx].track_id;
    if (cur_id == -1) {
      continue;
    }
    if (upload_struct_.count(cur_id) == 0) {
      InitUpLoadJson(conext_ptr, obj.detect_res_info[idx], detect_infos[idx]);
    } else {
      UpdataUpLoadJson(conext_ptr, obj.detect_res_info[idx], detect_infos[idx]);
    }
    SelectAttrofUploadJson(conext_ptr, obj.detect_res_info[idx], detect_infos[idx]);
//    const char *json_str = json_val_.dump().c_str();
  }
  std::fstream file("key.json");
  file << std::setw(4) << jsonfile;
  return true;
}

bool UploadDerocatestage::InitUpLoadJson(const contextPtr &conext_ptr,
                   const detect_bbox_info &detect, const PersonInfo &person) {
  json json_val_;
  json_val_["statusCode"] = status_Success;
  json_val_["streamId"] = conext_ptr->stream_id_;
  json_val_["equipmentId"] = conext_ptr->equipment_id_;
  json_val_["id"] = detect.track_id;
  json_val_["startTimestamp"] = conext_ptr->time_stamp_;
  json_val_["bodyIsObserved"] = true;
  cv::Mat src = cv::Mat(conext_ptr->input_h,conext_ptr->input_w,
                        CV_8UC3, conext_ptr->ori_data[0].data());
  cv::Mat roi = src(cv::Rect(detect.box.x1, detect.box.y1,
                         detect.box.x2 - detect.box.x1,
                         detect.box.y2 - detect.box.y1));
  json_val_["bodyInfo"]["croppedPicBase64"] = base64_encode(roi.data, roi.rows * roi.cols *3);
  json_val_["bodyInfo"]["feat"] = detect.feature;
  json_val_["bodyInfo"]["box"] = {detect.box.x1, detect.box.y1, detect.box.x2,detect.box.y2};
  json_val_["bodyInfo"]["selectedTimestamp"] = conext_ptr->time_stamp_;
  json_val_["bodyInfo"]["bodyProb"] = detect.prob;
  json_val_["bodyInfo"]["bodyTrailNum"] = 1;
  json_val_["bodyInfo"]["bodyTrailPointTime"].push_back(conext_ptr->time_stamp_);
  json_val_["bodyInfo"]["bodyTrailLeftTopX"].push_back(detect.box.x1);
  json_val_["bodyInfo"]["bodyTrailLeftTopY"].push_back(detect.box.y1);
  json_val_["bodyInfo"]["bodyTrailRightBtmX"].push_back(detect.box.x2);
  json_val_["bodyInfo"]["bodyTrailRightBtmY"].push_back(detect.box.y2);
  if (person.face_observeed) {
    json_val_["faceIsObserved"] = true;
    cv::Mat face = src(cv::Rect(person.face.x1, person.face.y1,
                                person.face.x2 - person.face.x1,
                                person.face.y2 - person.face.y1));
    json_val_["faceInfo"]["croppedPicBase64"] = base64_encode(face.data, face.rows * face.cols *3);;
    json_val_["faceInfo"]["feat"] ;
    json_val_["faceInfo"]["box"] = {person.face.x1, person.face.y1,person.face.x2, person.face.y2};
    json_val_["faceInfo"]["selectedTimestamp"] = conext_ptr->time_stamp_;
    json_val_["faceInfo"]["faceProb"] = person.face.conf;
  }
  if (!person.hand_packages.empty()) {
    for (int cnt = 0; cnt < person.hand_packages.size(); cnt++) {
      json_val_["handpackageInfo"]["handpackageTrailNum"] += 1;
      json_val_["handpackageInfo"]["handpackageTrailPointTime"].push_back(
        conext_ptr->time_stamp_);
      json_val_["handpackageInfo"]["handpackageTrailLeftTopX"];
      json_val_["handpackageInfo"]["handpackageTrailLeftTopY"];
      json_val_["handpackageInfo"]["handpackageTrailRightBtmX"];
      json_val_["handpackageInfo"]["handpackageTrailRightBtmY"];
    }
  }
  uint32_t cur_id = detect.track_id;
  upload_struct_[cur_id] = json_val_;
}

bool UploadDerocatestage::UpdataUpLoadJson(const contextPtr &conext_ptr,
                 const detect_bbox_info &detect, const PersonInfo &person) {
  uint32_t cur_id = detect.track_id;
  auto json_val = upload_struct_[cur_id];
  json_val["bodyInfo"]["bodyTrailNum"] += 1;
  json_val["bodyInfo"]["bodyTrailPointTime"].push_back(conext_ptr->time_stamp_);
  json_val["bodyInfo"]["bodyTrailLeftTopX"].push_back(detect.box.x1);
  json_val["bodyInfo"]["bodyTrailLeftTopY"].push_back(detect.box.y1);
  json_val["bodyInfo"]["bodyTrailRightBtmX"].push_back(detect.box.x2);
  json_val["bodyInfo"]["bodyTrailRightBtmY"].push_back(detect.box.y2);
  if (detect.prob > json_val["bodyInfo"]["bodyProb"]) {
    json_val["bodyInfo"]["bodyProb"] = detect.prob;
    json_val["bodyInfo"]["selectedTimestamp"] = conext_ptr->time_stamp_;
    json_val["bodyInfo"]["feat"] = detect.feature;
    json_val["faceInfo"]["box"] = {person.face.x1, person.face.y1,person.face.x2, person.face.y2};
    cv::Mat src = cv::Mat(conext_ptr->input_h,conext_ptr->input_w,
                          CV_8UC3, conext_ptr->ori_data[0].data());
    cv::Mat roi = src(cv::Rect(detect.box.x1, detect.box.y1,
                               detect.box.x2 - detect.box.x1,
                               detect.box.y2 - detect.box.y1));
    json_val["bodyInfo"]["croppedPicBase64"] = base64_encode(roi.data, roi.rows * roi.cols *3);
  }

  //TODO : add person.face.conf is whether is null;

  if (person.face.conf > json_val["faceInfo"]["faceProb"]) {
    cv::Mat src = cv::Mat(conext_ptr->input_h,conext_ptr->input_w,
                          CV_8UC3, conext_ptr->ori_data[0].data());
    cv::Mat face = src(cv::Rect(person.face.x1, person.face.y1,
                                person.face.x2 - person.face.x1,
                                person.face.y2 - person.face.y1));
    json_val["faceInfo"]["croppedPicBase64"] = base64_encode(face.data, face.rows * face.cols *3);;
    json_val["faceInfo"]["feat"] ;
    json_val["faceInfo"]["box"] = {person.face.x1, person.face.y1,person.face.x2, person.face.y2};
    json_val["faceInfo"]["selectedTimestamp"] = conext_ptr->time_stamp_;
    json_val["faceInfo"]["faceProb"] = person.face.conf;
  }
  if (!person.hand_packages.empty()) {
    for (int cnt = 0; cnt < person.hand_packages.size(); cnt++) {
      json_val["handpackageInfo"]["handpackageTrailNum"] += 1;
      json_val["handpackageInfo"]["handpackageTrailPointTime"].push_back(
        conext_ptr->time_stamp_);
      json_val["handpackageInfo"]["handpackageTrailLeftTopX"].push_back(person.hand_packages[cnt].x1);
      json_val["handpackageInfo"]["handpackageTrailLeftTopY"].push_back(person.hand_packages[cnt].y1);
      json_val["handpackageInfo"]["handpackageTrailRightBtmX"].push_back(person.hand_packages[cnt].x2);
      json_val["handpackageInfo"]["handpackageTrailRightBtmY"].push_back(person.hand_packages[cnt]. y2);
    }
  }
}
bool UploadDerocatestage::SelectAttrofUploadJson(const contextPtr &conext_ptr,
                     const detect_bbox_info &detect, const PersonInfo &person) {
  uint32_t cur_id = detect.track_id;
  auto json_val = upload_struct_[cur_id];
  if (cur_id == -1) {
    return true;
  }
  std::vector<cv::Point2f> bbox;

  for (int regionIdx = 0; regionIdx < stream_info_->config.size(); regionIdx++) {
    bool is_in_poly = false;
    for (int j = 0; j < bbox.size(); j++) {
      is_in_poly =
        PointInPolygon(bbox[j], stream_info_->config[regionIdx].polygon) ||
        is_in_poly;
    }
    if (is_in_poly) {
      std::string shape_type = stream_info_->config[regionIdx].shapeType;
      json_val["attributes"][shape_type] += 1;
//      json_val["regionSnapshots"][shape_type]["imgae"];
      if(json_val["regionSnapshots"][shape_type]["startTimestamp"].empty()) {
        json_val["regionSnapshots"][shape_type]["startTimestamp"] = conext_ptr->time_stamp_;
      }
      json_val["regionSnapshots"][shape_type]["endTimestamp"] = conext_ptr->time_stamp_;
    }
  }
}

REG_Stage(upload, std::make_shared<UploadDerocatestage>())
}  // namespace pipeline