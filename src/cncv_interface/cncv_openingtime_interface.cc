//
// Created by yankai.yan on 2021/6/17.
//

#include "cncv_openingtime_interface.h"
#include <string>
#include "json/json.hpp"

using json = nlohmann::json;

int CNCVOpeningTimeInit(char *json_str) {
  json json_val = json::parse(json_str);

  if (!json_val.contains("model_config")) {
    return -2;
  }
  std::string model_config = json_val["model_config"].get<std::string>();

  std::string model_type;
  if (json_val.contains("model_type")) {
    model_type = json_val["model_type"].get<std::string>();
  }
}

int CNCVOpeningTimeProcess(char *json_str) {
  json json_val = json::parse(json_str);
  std::string equipmentId;
  std::string traceId;
  std::string imageId;
  long timestamp;
  for (auto item : json_val) {
    if (item.contains("equipmentId")) {
      equipmentId = item["equipmentId"].get<std::string>();
    }
    if (!item.contains("image")) {
      return -2;
    }
    json image_info = item["image"];
    if (!image_info.contains("encodeData")) {
      return -2;
    }
    std::string encode_img = image_info["encodeData"].get<std::string>();

    if (!image_info.contains("width")) {
      return -2;
    }
    int width_img = image_info["width"].get<int>();

    if (!image_info.contains("height")) {
      return -2;
    }
    int height_img = image_info["height"].get<int>();

    if (!image_info.contains("channel")) {
      return -2;
    }
    int chn_img = image_info["channel"].get<int>();

    if (!image_info.contains("traceId")) {
      traceId = image_info["traceId"].get<std::string>();
    }
    if (!image_info.contains("imageId")) {
      imageId = image_info["imageId"].get<std::string>();
    }
    if (!image_info.contains("timestamp")) {
      timestamp = image_info["timestamp"].get<long>();
    }

  }
}