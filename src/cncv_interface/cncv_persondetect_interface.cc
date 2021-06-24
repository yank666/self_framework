//
// Created by yankai.yan on 2021/6/17.
//

#include "cncv_persondetect_interface.h"
#include <memory>
#include "opencv2/opencv.hpp"
#include "glog/logging.h"
#include "base64encoding/base64.h"
#include "json/json.hpp"
#include "pipeline/pipeline.h"
#include "utils/base.h"
#include "parse/parse_config.h"

using json = nlohmann::json;
pipeline::pipelinePtr pipeline_ptr;
constexpr int CNCV_VERSION_MAJOR = 1;
constexpr int CNCV_VERSION_MINOR = 0;
constexpr int CNCV_VERSION_PATCH = 0;
constexpr char CNCV_BUILD_TIME[] = " Built:2021-06-30 00:00:00";

std::string ver_str;
const char* CNCVPersonDetectVersion() {
  ver_str = "v" + std::to_string(CNCV_VERSION_MAJOR) + "." +
            std::to_string(CNCV_VERSION_MINOR) + "." +
            std::to_string(CNCV_VERSION_PATCH) + CNCV_BUILD_TIME;
  return ver_str.c_str();
}

int CNCVPersonDetectInit(char *json_str) {
  LOG(INFO) << "CNCVPersonDetectInit run success, pipeline has inited!";
  return 0;
  json json_val = json::parse(json_str);

  if (!json_val.contains("model_config")) {
    return -2;
  }
  std::string model_config = json_val["model_config"].get<std::string>();

  std::string model_type;
  if (json_val.contains("model_type")) {
    model_type = json_val["model_type"].get<std::string>();
  }

  std::vector<pipeline::ModelCfgPtr> cfg_vec;
  if (pipeline::ParseConfig::ParseConfigFromProto(model_config, &cfg_vec) != 0) {
    LOG(ERROR) << "Parse model config failed!";
  }
  pipeline_ptr = std::make_shared<pipeline::Pipeline>();
  REPORT_EXCEPTION_IF_NULL(pipeline_ptr);
  pipeline_ptr->InitPipeline(cfg_vec);
}

int CNCVInitImageInfo(const json &image_info) {
  std::string equipmentId;
  std::string streamId;
  int imageId;
  long timestamp;
  if (!image_info.contains("streamId")) {
    streamId = image_info["traceId"].get<std::string>();
  }
  if (image_info.contains("equipmentId")) {
    equipmentId = image_info["equipmentId"].get<std::string>();
  }
  if (!image_info.contains("imageId")) {
    imageId = image_info["imageId"].get<int>();
  }
  if (!image_info.contains("timestamp")) {
    timestamp = image_info["timestamp"].get<long>();
  }
  pipeline_ptr->SetHardwareofContext(streamId, equipmentId, imageId, timestamp);
}

char *CNCVPersonDetectProcess(const CNCVImageInfo &img_info, char *json_str) {
  LOG(INFO) << "CNCVPersonDetectProcess run success, pipeline begin run!";
  json json_val1;
  return nullptr;
  json json_val = json::parse(json_str);
  std::vector<uint32_t> input_size;
  for (auto item : json_val) {
    if (!item.contains("image")) {
      return nullptr;
    }
    json image_info = item["image"];
    if (!image_info.contains("encodeData")) {
      return nullptr;
    }
    std::string encode_img = image_info["encodeData"].get<std::string>();
    std::string decode_img = base64_decode(encode_img);
    std::vector<char> base64_img(decode_img.begin(), decode_img.end());
    cv::Mat input_img = cv::imdecode(base64_img, CV_LOAD_IMAGE_COLOR);
    if (!image_info.contains("width")) {
      return nullptr;
    }
    int width_img = image_info["width"].get<int>();

    if (!image_info.contains("height")) {
      return nullptr;
    }
    int height_img = image_info["height"].get<int>();

    if (!image_info.contains("channel")) {
      return nullptr;
    }
    int chn_img = image_info["channel"].get<int>();
    input_size.push_back(width_img * height_img * chn_img * sizeof(uint8_t));
    CNCVInitImageInfo(image_info);
    pipeline_ptr->PushDatatoPipeline((char **)&(input_img.data), input_size,
                                     width_img, height_img);
  }
  return 0;
}

int CNCVPersonDetectDeinit() {
  LOG(INFO) << "CNCVPersonDetectDeinit run success, pipeline had delete!";
  return 0;
  pipeline_ptr.reset();
};
