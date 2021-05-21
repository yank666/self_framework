//
// Created by yankai.yan on 2021/5/17.
//

#ifndef SELF_ARCHITECTURE_UPLOAD_MANAGER_H
#define SELF_ARCHITECTURE_UPLOAD_MANAGER_H
#include <memory>
#include "src/utils/errorcode.h"
#include "opencv2/opencv.hpp"

namespace pipeline {
struct RegionInfo {
  std::string areaId;
  std::string shapeType;
  std::string algoType;
  std::vector<cv::Point2d> polygon;
};

struct StreamInfo {
  std::string streamId;
  std::string equipmentId;
  std::string taskType;
  std::vector<RegionInfo> config;
};


class ParseJson {
 public:
  ParseJson() = default;
  ~ParseJson() = default;
  static STATUS ParseInitJson(char* input);
  static STATUS ParseVideoJson(char* input,
                               const std::shared_ptr<StreamInfo> &stream_info);
};
}
#endif  // SELF_ARCHITECTURE_UPLOAD_MANAGER_H
