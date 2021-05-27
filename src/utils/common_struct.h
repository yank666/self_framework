//
// Created by yankai.yan on 2021/5/17.
//

#ifndef SELF_ARCHITECTURE_COMMON_STRUCT_H
#define SELF_ARCHITECTURE_COMMON_STRUCT_H
#include "opencv2/opencv.hpp"

struct YoloV5Box {
  float x1;
  float y1;
  float x2;
  float y2;
  float conf;
};

struct YoloV5BodyInfo {
  int type;
  YoloV5Box box;
  YoloV5Box related_box;
  std::vector<std::pair<int, float>> attrs;  // 属性label: 属性score
};

struct PersonInfo {
  int32_t label;
  YoloV5Box body;
  YoloV5Box head;
  YoloV5Box face;

  std::vector<float> feature;
  bool face_observeed = false;
  bool head_observed = false;
  YoloV5Box hand_packages[10];
  uint8_t hand_packages_nums{0};
  std::pair<int, float> attrs[4];
};

enum VCSStatusCode {
  status_Success = 1,  //成功、正常
  status_Starting = 2,  // 启动拉流中，接收到start请求但是还没有到正常的状态
  status_Stopping = 3,  // 停止分析中，接受到stop请求， 但是无法到停止的状态中
  status_ErrInputParam = 250,  //输入参数异常
  status_LicenseError = 252,   // license 失效
  status_ExceedCap = 254,      // 超出分析能力
  status_Error = 255,          //码流异常， 无码流
  status_Size_error = 256,     // 尺寸不支持。
  status_UnknowError,          //未知异常
};

struct cncvObjAttributes {
  int shelf;
  int checkOut;
  int checkOutCore;
  int exit;
};

struct cncvBodyInfo {
  cv::Mat croppedPicBase64;
  std::vector<float> feat;
  std::vector<float> box;
  long long selectedTimestamp;
  float bodyProb;
  float bodyQuality;
  int bodyTrailNum;
  std::vector<long long> bodyTrailPointTime;
  std::vector<float> bodyTrailLeftTopX;
  std::vector<float> bodyTrailLeftTopY;
  std::vector<float> bodyTrailRightBtmX;
  std::vector<float> bodyTrailRightBtmY;
};

struct cncvFaceInfo {
  cv::Mat croppedPicBase64;
  long img_size;
  std::vector<float> box;
  std::vector<float> feat;
  long long selectedTimestamp;
  float faceProb;
  float faceQuality;
  float faceKeypointQuality;
};

struct cncvHandPackageInfo {
  int handpackageTrailNum;
  std::vector<long long> handpackageTrailPointTime;
  std::vector<float> handpackageTrailLeftTopX;
  std::vector<float> handpackageTrailLeftTopY;
  std::vector<float> handpackageTrailRightBtmX;
  std::vector<float> handpackageTrailRightBtmY;
};

struct cncvRegionSnapshot {
  cv::Mat image;
  long image_size;
  long long startTimestamp;
  long long endTimestamp;
  float snapShotQuality;
};

struct cncvRegionSnapshots {
  cncvRegionSnapshot shelf;
  cncvRegionSnapshot checkOut;
  cncvRegionSnapshot checkOutCore;
  cncvRegionSnapshot exit;
};

struct UploadObj {
  VCSStatusCode statusCode;
  std::string id;
  std::string equipmentId;
  std::string streamId;
  long long startTimestamp;
  long long endTimestamp;
  bool bodyIsObserved;
  bool faceIsObserved;
  cncvObjAttributes attributes;
  cncvBodyInfo bodyInfo;
  cncvFaceInfo faceInfo;
  cncvHandPackageInfo handPackageInfo;
  cncvRegionSnapshots regionSnapshots;
};
#endif  // SELF_ARCHITECTURE_COMMON_STRUCT_H
