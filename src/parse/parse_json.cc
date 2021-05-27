//
// Created by yankai.yan on 2021/5/17.
//

#include "parse_json.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include "json/json.hpp"
#include "src/utils/base.h"
namespace pipeline {
using json = nlohmann::json;

STATUS ParseJson::ParseInitJson(char *input) {
  json json_val = json::parse(input);
  if (!json_val.contains("nGpuId")) {
    return RET_INPUT_PARAM_INVALID;
  }
  int n_gpuid = json_val["nGpuId"].get<int>();
  if (!json_val.contains("fps")) {
    return RET_INPUT_PARAM_INVALID;
  }
  int n_fps = json_val["fps"].get<int>();
  return RET_OK;
}

STATUS ParseJson::ParseVideoJson(
  char *input, const std::shared_ptr<StreamInfo> &stream_info) {
  REPORT_EXCEPTION_IF_NULL(stream_info);
  json json_val = json::parse(input);
  if (!json_val.contains("stream_width") ||
      !json_val.contains("stream_height")) {
    return RET_INPUT_PARAM_INVALID;
  }
  int width = json_val["stream_width"].get<int>();
  int height = json_val["stream_height"].get<int>();

  if (!json_val.contains("streamId")) {
    return RET_INPUT_PARAM_INVALID;
  }
  stream_info->streamId = json_val["streamId"].get<std::string>();
  if (!json_val.contains("equipmentId")) {
    return RET_INPUT_PARAM_INVALID;
  }
  stream_info->equipmentId = json_val["equipmentId"].get<std::string>();
  if (!json_val.contains("taskType")) {
    return RET_INPUT_PARAM_INVALID;
  }
  stream_info->taskType = json_val["taskType"].get<std::string>();

  if (!json_val.contains("config")) {
    return RET_INPUT_PARAM_INVALID;
  }
  json json_conf = json_val["config"];
  if (!json_conf.contains("Graphs")) {
    return RET_INPUT_PARAM_INVALID;
  }
  json json_sub = json_conf["Graphs"];
  for (auto it : json_sub) {
    RegionInfo region_info;
    if (!it.contains("shapeType")) {
      return RET_INPUT_PARAM_INVALID;
    }
    region_info.shapeType = it.at("shapeType").get<std::string>();
    if (!it.contains("algoType")) {
      return RET_INPUT_PARAM_INVALID;
    }
    region_info.algoType = it.at("algoType").get<std::string>();
    if (!it.contains("areaId")) {
      return RET_INPUT_PARAM_INVALID;
    }
    region_info.areaId = it.at("areaId").get<std::string>();

    if (!it.contains("shapeList")) {
      return RET_INPUT_PARAM_INVALID;
    }
    json json_array = it.at("shapeList");
    for (auto item : json_array) {
      if (!item.contains("x") || !item.contains("y")) {
        return RET_INPUT_PARAM_INVALID;
      }
      cv::Point2d point;
      point.x = std::stod(item.at("x").get<std::string>()) * width;
      point.y = std::stod(item.at("y").get<std::string>()) * height;
      region_info.polygon.emplace_back(point);
    }
    stream_info->config.push_back(region_info);
  }
  return RET_OK;
}
}  // namespace pipeline