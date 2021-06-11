//
// Created by yankai.yan on 2021/4/15.
//

#include "yolo_decorator_stage.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <cfloat>
#include <iterator>
#include "glog/logging.h"
#include "src/utils/base.h"
#include "src/pipeline/stage_register.h"

namespace pipeline {
inline float StrictScale(float input, float max) {
  float ret = (input < 0) ? 0 : input;
  return (ret > max) ? max : ret;
}

const float kConfThreshold = 0.3f;
const float kIouThreshold = 0.5f;
const float kBoxThreshold = 0.3f;
const uint32_t kTotalOut = 333396;

enum YoloInputDesc : int {
  kInb = 1,
  kInc = 3,
  kInd = 4,
  kInh = 384,
  kInw = 672
};
enum YoloOutputDesc : uint32_t {
  kOutCnt = 3,
  kOutDim = 4,
  kClassifyNum = 4,
  kAnchorDim = 6,
  kOutUnit = 21
};
enum YoloOutputIdx : int {
  Idx_x = 0,
  Idx_y,
  Idx_w,
  Idx_h,
  Idx_conf,
  Idx_c1,
  Idx_c2,
  Idx_c3,
  Idx_c4,
  Idx_Rconf,
  Idx_Rcx,
  Idx_Rcy,
  Idx_Rrx,
  Idx_Rdy
};

static const uint32_t kYoloV5Anchor[kOutCnt][kAnchorDim] = {
  {10, 13, 16, 30, 33, 23},
  {30, 61, 62, 45, 59, 119},
  {116, 90, 156, 198, 373, 326}};
static const uint32_t kYoloV5Strie[] = {8, 16, 32};

const std::vector<std::vector<uint32_t>> kOutActualShape{
  {1, 3, 4032, kOutUnit}, {1, 3, 1008, kOutUnit}, {1, 3, 252, kOutUnit}};
const std::vector<std::vector<uint32_t>> kYoloV5ShapeNp{
  {1, 3, 4032, 2}, {1, 3, 1008, 2}, {1, 3, 252, 2}};

static const uint32_t kConfiIdx = 4;

enum YoloDimDesc : int { idx_batch = 0, idx_channel = 1, idx_h = 2, idx_w = 3 };

enum ClassifyLabel : int {
  cls_person = 0,
  cls_shoulder,
  cls_face,
  cls_handpackage
};

inline void CopyAttrArray(std::pair<int, float> *dst,
                          const std::vector<std::pair<int, float>> src) {
  for (auto item : src) {
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[4];
  }
}

inline float sigmoid(float x) {
  float temp = (1 / (1 + exp(-x)));
  return temp;
}

inline float bbox_iou(const YoloV5Box &box1, const YoloV5Box &box2) {
  float xx1 = std::max<float>(box1.x1, box2.x1);
  float yy1 = std::max<float>(box1.y1, box2.y1);
  float xx2 = std::min<float>(box1.x2, box2.x2);
  float yy2 = std::min<float>(box1.y2, box2.y2);
  float w = std::max<float>(0, xx2 - xx1);
  float h = std::max<float>(0, yy2 - yy1);
  float area = w * h;
  float combine_area = (box1.x2 - box1.x1) * (box1.y2 - box1.y1) +
                       (box2.x2 - box2.x1) * (box2.y2 - box2.y1);
  if (std::fabs(combine_area - area) <= FLT_EPSILON) {
    return 0;
  }
  float iou = area / (combine_area - area);
  return iou;
}

void softmax(float *x, int n) {
  float sum = 0.0;
  for (int i = 0; i < n; ++i) {
    x[i] = exp(x[i]);
    sum += x[i];
  }
  for (int i = 0; i < n; ++i) {
    x[i] /= sum;
  }
}

void maxscore(const float *x, int n, float &max_score, int &max_ind) {
  max_score = -1;
  max_ind = 0;
  for (int i = 0; i < n; i++) {
    if (x[i] > max_score) {
      max_score = x[i];
      max_ind = i;
    }
  }
}

static bool CompareBBox(const YoloV5BodyInfo &a, const YoloV5BodyInfo &b) {
  return a.box.conf > b.box.conf;
}

bool YoloDerocatestage::StagePreProcess(const contextPtr &conext_ptr,
                                        const ProcessContextMap &contextMap) {
  REPORT_ERROR_IF_NULL(conext_ptr);
  TimeWatch time_watch;
  time_watch.start();
  MakeGridNp();

  DLOG(INFO) << "Run Yolo PostDecorate stage run success, cost"
            << time_watch.stop() << "ms";
  return true;
}

void YoloDerocatestage::MakeGridNp() {
  if (anchor_grid_ != nullptr && anchor_grid_np_ != nullptr) {
    return;
  }
  anchor_grid_ = std::make_shared<std::vector<std::vector<int32_t>>>();
  anchor_grid_np_ = std::make_shared<std::vector<std::vector<int32_t>>>();

  anchor_grid_->resize(kOutCnt);
  anchor_grid_np_->resize(kOutCnt);
  for (size_t i = 0; i < kOutCnt; i++) {
    int32_t x_dims = kInw / kYoloV5Strie[i];
    int32_t y_dims = kInh / kYoloV5Strie[i];

    for (size_t x = 0; x < y_dims; ++x) {
      for (size_t y = 0; y < x_dims; ++y) {
        anchor_grid_->at(i).push_back(y);
        anchor_grid_->at(i).push_back(x);
      }
    }

    size_t grid_size = anchor_grid_->at(i).size();
    for (size_t k = 1; k < kOutCnt; ++k) {
      anchor_grid_->at(i).insert(anchor_grid_->at(i).end(),
                                 anchor_grid_->at(i).begin(),
                                 anchor_grid_->at(i).begin() + grid_size);
    }

    for (size_t num = 0; num < kOutCnt; num++) {
      for (size_t idx = 0; idx < x_dims * y_dims; ++idx) {
        for (size_t count = 0; count < kAnchorDim / kOutCnt; count++) {
          anchor_grid_np_->at(i).push_back(kYoloV5Anchor[i][num * 2 + count]);
        }
      }
    }
  }
}

void YoloDerocatestage::DecodingOutput(float **inputarray,
                                       std::vector<float> *output) {
  std::vector<std::vector<float>> output_y(kOutCnt);
  std::vector<std::vector<int32_t>> output_grid_np;
  output_grid_np.resize(kOutCnt);
  std::vector<std::vector<int32_t>> output_anchor_grid_np;
  output_anchor_grid_np.resize(kOutCnt);
  bool conf = false;
  for (size_t i = 0; i < kOutCnt; ++i) {
    for (size_t batch = 0; batch < kOutActualShape[i][idx_batch]; ++batch) {
      for (size_t channel = 0; channel < kOutActualShape[i][idx_channel];
           ++channel) {
        for (size_t height = 0; height < kOutActualShape[i][idx_h]; ++height) {
          uint32_t idx =
            height * kOutActualShape[i][idx_w] +
            channel * kOutActualShape[i][idx_h] * kOutActualShape[i][idx_w] +
            batch * kOutActualShape[i][idx_channel] *
              kOutActualShape[i][idx_h] * kOutActualShape[i][idx_w];

          // cx,cy,w,h,conf,c1,c2,c3,c4输出做sigmoid
          for (int start = 0; start < kClassifyNum + 5; start++) {
            *(inputarray[i] + idx + start) =
              sigmoid(*(inputarray[i] + idx + start));
          }
          // cx,cy,w,h,conf,c1,c2,c3,c4, rconf, rc1,rc2,rc3,rc4
          *(inputarray[i] + idx + kClassifyNum + 5) =
            sigmoid(*(inputarray[i] + idx + kClassifyNum + 5));
          // 朝向属性 a0, a1, a2, a3做softmax
          softmax(inputarray[i] + idx + kClassifyNum + 5 + 5, 4);
          // 其他属性做sigmoid
          for (int start = kClassifyNum + 5 + 5 + 4; start < kOutUnit;
               start++) {
            *(inputarray[i] + idx + start) =
              sigmoid(*(inputarray[i] + idx + start));
          }

          conf = *(inputarray[i] + idx + kConfiIdx) >= kConfThreshold;

          if (conf) {
            output_y[i].insert(output_y[i].end(), inputarray[i] + idx,
                               inputarray[i] + idx + kOutActualShape[i][3]);
            uint32_t idx_np =
              height * kYoloV5ShapeNp[i][3] +
              channel * kYoloV5ShapeNp[i][2] * kYoloV5ShapeNp[i][3] +
              batch * kYoloV5ShapeNp[i][1] * kYoloV5ShapeNp[i][2] *
                kYoloV5ShapeNp[i][3];
            output_grid_np[i].insert(output_grid_np[i].end(),
                                     anchor_grid_->at(i).begin() + idx_np,
                                     anchor_grid_->at(i).begin() + idx_np + 2);
            output_anchor_grid_np[i].insert(
              output_anchor_grid_np[i].end(),
              anchor_grid_np_->at(i).begin() + idx_np,
              anchor_grid_np_->at(i).begin() + idx_np + 2);
          }
        }
      }
    }
  }

  for (size_t i = 0; i < kOutCnt; ++i) {
    for (size_t idx = 0; idx * kOutActualShape[i][3] < output_y[i].size();
         ++idx) {
      float temp = output_y[i][idx * kOutActualShape[i][3] + 0];
      float temp1 = output_grid_np[i][idx * 2 + 0];
      output_y[i][idx * kOutActualShape[i][3] + 0] =
        (output_y[i][idx * kOutActualShape[i][3] + 0] * 2.0 - 0.5 +
         output_grid_np[i][idx * 2 + 0]) *
        kYoloV5Strie[i];
      output_y[i][idx * kOutActualShape[i][3] + 1] =
        (output_y[i][idx * kOutActualShape[i][3] + 1] * 2.0 - 0.5 +
         output_grid_np[i][idx * 2 + 1]) *
        kYoloV5Strie[i];
      output_y[i][idx * kOutActualShape[i][3] + 2] =
        std::pow(output_y[i][idx * kOutActualShape[i][3] + 2] * 2.0, 2) *
        output_anchor_grid_np[i][idx * 2 + 0];
      output_y[i][idx * kOutActualShape[i][3] + 3] =
        std::pow(output_y[i][idx * kOutActualShape[i][3] + 3] * 2.0, 2) *
        output_anchor_grid_np[i][idx * 2 + 1];

      // cx,cy,w,h,conf,c1,c2,c3,c4, rconf, rc1,rc2,rc3,rc4
      output_y[i][idx * kOutActualShape[i][3] + kClassifyNum + 5 + 1] =
        output_y[i][idx * kOutActualShape[i][3] + kClassifyNum + 5 + 1] *
          output_anchor_grid_np[i][idx * 2 + 0] +
        output_grid_np[i][idx * 2 + 0] * kYoloV5Strie[i];
      output_y[i][idx * kOutActualShape[i][3] + kClassifyNum + 5 + 2] =
        output_y[i][idx * kOutActualShape[i][3] + kClassifyNum + 5 + 2] *
          output_anchor_grid_np[i][idx * 2 + 1] +
        output_grid_np[i][idx * 2 + 1] * kYoloV5Strie[i];
      output_y[i][idx * kOutActualShape[i][3] + kClassifyNum + 5 + 3] =
        output_y[i][idx * kOutActualShape[i][3] + kClassifyNum + 5 + 3] *
          output_anchor_grid_np[i][idx * 2 + 0] +
        output_grid_np[i][idx * 2 + 0] * kYoloV5Strie[i];
      output_y[i][idx * kOutActualShape[i][3] + kClassifyNum + 5 + 4] =
        output_y[i][idx * kOutActualShape[i][3] + kClassifyNum + 5 + 4] *
          output_anchor_grid_np[i][idx * 2 + 1] +
        output_grid_np[i][idx * 2 + 1] * kYoloV5Strie[i];

      output_y[i][idx * kOutActualShape[i][3] + kClassifyNum + 5 + 3] =
        (output_y[i][idx * kOutActualShape[i][3] + kClassifyNum + 5 + 3] -
         output_y[i][idx * kOutActualShape[i][3] + kClassifyNum + 5 + 1]) *
        2;
      output_y[i][idx * kOutActualShape[i][3] + kClassifyNum + 5 + 4] =
        (output_y[i][idx * kOutActualShape[i][3] + kClassifyNum + 5 + 4] -
         output_y[i][idx * kOutActualShape[i][3] + kClassifyNum + 5 + 2]) *
        2;
    }
    output->insert(output->end(), output_y[i].begin(), output_y[i].end());
  }
}

void YoloDerocatestage::ObtainBoxAndScore(
  const std::vector<float> &output,
  std::vector<std::vector<YoloV5BodyInfo>> *nms_boxes_vec) {
  std::vector<std::vector<YoloV5BodyInfo>> vec_boxes;
  vec_boxes.resize(kClassifyNum);
  for (size_t idx = 0; idx * kOutActualShape[0][3] < output.size(); ++idx) {
    int32_t max_score_idx = -1;
    float score_max = 0.0f;
    for (uint32_t score_idx = 1; score_idx < kClassifyNum + 1; ++score_idx) {
      float score_temp =
        output[idx * kOutActualShape[0][3] + kConfiIdx] *
        output[idx * kOutActualShape[0][3] + kConfiIdx + score_idx];
      if (score_temp >= score_max) {
        score_max = score_temp;
        max_score_idx = score_idx;
      }
    }
    YoloV5BodyInfo body_info;
    body_info.box.conf = score_max;
    body_info.box.x1 =
      StrictScale(output[idx * kOutActualShape[0][3] + 0] -
                    output[idx * kOutActualShape[0][3] + 2] / 2,
                  kInw);
    body_info.box.y1 =
      StrictScale(output[idx * kOutActualShape[0][3] + 1] -
                    output[idx * kOutActualShape[0][3] + 3] / 2,
                  kInh);
    body_info.box.x2 = StrictScale(
      body_info.box.x1 + output[idx * kOutActualShape[0][3] + 2], kInw);
    body_info.box.y2 = StrictScale(
      body_info.box.y1 + output[idx * kOutActualShape[0][3] + 3], kInh);
    body_info.related_box.conf =
      output[idx * kOutActualShape[0][3] + kClassifyNum + 5];
    body_info.related_box.x1 = StrictScale(
      output[idx * kOutActualShape[0][3] + kClassifyNum + 5 + 1] -
        output[idx * kOutActualShape[0][3] + kClassifyNum + 5 + 3] / 2,
      kInw);
    body_info.related_box.y1 = StrictScale(
      output[idx * kOutActualShape[0][3] + kClassifyNum + 5 + 2] -
        output[idx * kOutActualShape[0][3] + kClassifyNum + 5 + 4] / 2,
      kInh);
    body_info.related_box.x2 =
      StrictScale(body_info.related_box.x1 +
                    output[idx * kOutActualShape[0][3] + kClassifyNum + 5 + 3],
                  kInw);;
    body_info.related_box.y2 =
      StrictScale(body_info.related_box.y1 +
                    output[idx * kOutActualShape[0][3] + kClassifyNum + 5 + 4],
                  kInh);
    float orient_max_score = -1.0;
    int orient_max_ind = 0;
    maxscore(&output[idx * kOutActualShape[0][3] + kClassifyNum + 5 + 5], 4,
             orient_max_score, orient_max_ind);
    body_info.attrs.push_back(
      std::pair<int, float>(orient_max_ind, orient_max_score));  // 朝向
    body_info.attrs.push_back(
      std::pair<int, float>(0, output[idx * kOutActualShape[0][3] +
                                      kClassifyNum + 5 + 5 + 4]));  // 可见程度
    body_info.attrs.push_back(
      std::pair<int, float>(0, output[idx * kOutActualShape[0][3] +
                                      kClassifyNum + 5 + 5 + 4 + 1]));  // 遮挡
    body_info.attrs.push_back(std::pair<int, float>(
      0, output[idx * kOutActualShape[0][3] + kClassifyNum + 5 + 5 + 4 +
                2]));  // 清晰度
    vec_boxes[max_score_idx - 1].emplace_back(body_info);
  }

  nms_boxes_vec->resize(kClassifyNum);
  for (size_t i = 0; i < kClassifyNum; ++i) {
    NMSProcess(vec_boxes[i], &(nms_boxes_vec->at(i)));
  }
}

void YoloDerocatestage::NMSProcess(const std::vector<YoloV5BodyInfo> &vec_boxes,
                                   std::vector<YoloV5BodyInfo> *bboxes_nms) {
  if (kIouThreshold < 0.0f) {
    LOG(ERROR) << "threshold in NMSProcess is wrong.";
  }
  std::vector<YoloV5BodyInfo> bboxes(vec_boxes);
  std::sort(bboxes.begin(), bboxes.end(), CompareBBox);
  int32_t select_idx = 0;
  int32_t num_bbox = static_cast<int32_t>(bboxes.size());
  std::vector<int32_t> mask_merged(num_bbox, 0);
  bool all_merged = false;

  while (!all_merged) {
    while (select_idx < num_bbox && mask_merged[select_idx] == 1) select_idx++;
    if (select_idx == num_bbox) {
      all_merged = true;
      continue;
    }
    bboxes_nms->push_back(bboxes[select_idx]);
    mask_merged[select_idx] = 1;
    YoloV5Box select_bbox = bboxes[select_idx].box;
    float area1 = static_cast<float>((select_bbox.x2 - select_bbox.x1 + 1) *
                                     (select_bbox.y2 - select_bbox.y1 + 1));
    float x1 = static_cast<float>(select_bbox.x1);
    float y1 = static_cast<float>(select_bbox.y1);
    float x2 = static_cast<float>(select_bbox.x2);
    float y2 = static_cast<float>(select_bbox.y2);

    select_idx++;
    for (int32_t i = select_idx; i < num_bbox; i++) {
      if (mask_merged[i] == 1) continue;

      YoloV5Box &bbox_i = bboxes[i].box;
      float x = std::max<float>(x1, static_cast<float>(bbox_i.x1));
      float y = std::max<float>(y1, static_cast<float>(bbox_i.y1));
      float w = std::min<float>(x2, static_cast<float>(bbox_i.x2)) - x + 1;
      float h = std::min<float>(y2, static_cast<float>(bbox_i.y2)) - y + 1;
      if (w <= 0 || h <= 0) continue;

      float area2 = static_cast<float>((bbox_i.x2 - bbox_i.x1 + 1) *
                                       (bbox_i.y2 - bbox_i.y1 + 1));
      float area_intersect = w * h;

      if (static_cast<float>(area_intersect) /
            (area1 + area2 - area_intersect) >
          kIouThreshold) {
        mask_merged[i] = 1;
      }
    }
  }
}

std::shared_ptr<std::vector<PersonInfo>> YoloDerocatestage::ComputeAssociate(
  std::vector<std::vector<YoloV5BodyInfo>> *nms_boxes) {
  std::shared_ptr<std::vector<PersonInfo>> associated_detections =
    std::make_shared<std::vector<PersonInfo>>();

  size_t num_body = nms_boxes->at(0).size();
  size_t num_head = nms_boxes->at(1).size();
  size_t num_face = nms_boxes->at(2).size();
  size_t num_package = nms_boxes->at(3).size();

  Eigen::MatrixXf Cost_body2head = Eigen::MatrixXf::Zero(num_body, num_head);
  for (size_t i = 0; i < num_body; ++i) {
    for (size_t j = 0; j < num_head; ++j) {
      float iou =
        bbox_iou(nms_boxes->at(0)[i].related_box, nms_boxes->at(1)[j].box);
      // float cost_pos = bbox_dist(body_detections[i]->GetAssociatedBox(),
      // head_detections[j]->GetAssociatedBox());
      Cost_body2head(i, j) = 1 - iou;
    }
  }
  std::unordered_map<size_t, size_t> matched_pairs;
  std::vector<size_t> unmatched_rows, unmatched_cols;
  FindAssociationPairs(Cost_body2head, 0.8, matched_pairs, unmatched_rows,
                       unmatched_cols);

  for (const auto &iter : matched_pairs) {
    auto body_i = iter.first;
    auto head_j = iter.second;

    PersonInfo person_info;
    person_info.label = 0;
    CopyAttrArray(person_info.attrs, nms_boxes->at(0)[body_i].attrs);
    person_info.body = nms_boxes->at(0)[body_i].box;
    person_info.head = nms_boxes->at(1)[head_j].box;
    person_info.body.conf = nms_boxes->at(0)[body_i].box.conf;
    person_info.head.conf = nms_boxes->at(1)[head_j].related_box.conf;
    person_info.head_observed = true;
    associated_detections->push_back(person_info);
  }

  // body匹配不成功，保留body还是直接删掉??
  for (const auto body_i : unmatched_rows) {
    PersonInfo person_info;
    person_info.label = 0;
    CopyAttrArray(person_info.attrs, nms_boxes->at(0)[body_i].attrs);
    person_info.body.conf = nms_boxes->at(0)[body_i].box.conf;
    person_info.head.conf = nms_boxes->at(0)[body_i].related_box.conf;
    //    person_info.attr = body_detections[body_i]->attr;

    person_info.body = nms_boxes->at(0)[body_i].box;
    person_info.head = nms_boxes->at(0)[body_i].related_box;

    // 防止head_width=0 || head_height=0
    if ((person_info.head.x2 - person_info.head.x1) < 2 ||
        (person_info.head.y2 - person_info.head.y1) < 2) {
      person_info.head.x1 = person_info.body.x1;
      person_info.head.y1 = person_info.body.y1;
      person_info.head.x2 = person_info.body.x2;
      person_info.head.y2 =
        person_info.body.y1 + (person_info.body.y2 - person_info.body.y1) / 3;
    }
    associated_detections->push_back(person_info);
  }

  int num_person = associated_detections->size();
  // 2.0 face 与 head 关联
  Eigen::MatrixXf Cost_face2head = Eigen::MatrixXf::Zero(num_face, num_person);
  for (size_t i = 0; i < num_face; ++i) {
    for (size_t j = 0; j < num_person; ++j) {
      float iou = bbox_iou(nms_boxes->at(2)[i].related_box,
                           associated_detections->at(j).head);
      Cost_face2head(i, j) = 1 - iou;
    }
  }
  FindAssociationPairs(Cost_face2head, 0.8, matched_pairs, unmatched_rows,
                       unmatched_cols);

  // # 只处理匹配成功的face
  for (const auto &iter : matched_pairs) {
    auto face_i = iter.first;
    auto person_j = iter.second;

    associated_detections->at(person_j).face_observeed = true;
    associated_detections->at(person_j).face = nms_boxes->at(2)[face_i].box;
  }

  // 3.0 handpackage 与 body 关联. 利用中心点距离进行关联
  // 一个person可能对应多个handpackage
  Eigen::MatrixXf Cost_package2body =
    Eigen::MatrixXf::Zero(num_package, num_person);
  for (size_t i = 0; i < num_package; ++i) {
    float min_dist = 10.0;
    size_t match_j = 0;
    for (size_t j = 0; j < num_person; ++j) {
      auto package_box = nms_boxes->at(3)[i].box;
      std::pair<double, double> package_associated_points =
        std::make_pair<double, double>((package_box.x1 + package_box.x2) / 2,
                                       (package_box.y1 + package_box.y2) / 2);
      auto body_box = associated_detections->at(j).body;  //
      std::pair<double, double> body_points = std::make_pair<double, double>(
        (body_box.x1 + body_box.x2) / 2, (body_box.y1 + body_box.y2) / 2);
      float body_diag = std::sqrt(std::pow(body_box.x2 - body_box.x1, 2) +
                                  std::pow(body_box.y2 - body_box.y1, 2));

      std::pair<double, double> diff = std::make_pair<double, double>(
        body_points.first - package_associated_points.first,
        body_points.second - package_associated_points.second);
      float dist =
        sqrtf(diff.first * diff.first + diff.second * diff.second) / body_diag;

      if (dist < min_dist) {
        min_dist = dist;
        match_j = j;
      }
    }

    if (min_dist < 0.5) {
      associated_detections->at(match_j)
        .hand_packages[associated_detections->at(match_j).hand_packages_nums] =
        nms_boxes->at(3)[i].box;
      associated_detections->at(match_j).hand_packages_nums++;
    }
  }

  return associated_detections;
}
void YoloDerocatestage::FilterBoxByScore(
  std::vector<std::vector<YoloV5BodyInfo>> *nms_boxes_vec) {
  std::for_each(nms_boxes_vec->begin(), nms_boxes_vec->end(),
                [](std::vector<YoloV5BodyInfo> &vec_info) {
                  auto iter = vec_info.begin();
                  while (iter != vec_info.end()) {
                    if (iter->box.conf < kBoxThreshold) {
                      iter = vec_info.erase(iter);
                    } else {
                      iter++;
                    }
                  }
                });
}
bool YoloDerocatestage::FindAssociationPairs(
  Eigen::MatrixXf &cost_matrix, float matching_gate,
  std::unordered_map<size_t, size_t> &matched_pairs,
  std::vector<size_t> &orphan_row_items,
  std::vector<size_t> &orphan_col_items) {
  matched_pairs.clear();
  orphan_row_items.clear();
  orphan_col_items.clear();

  if (cost_matrix.size() == 0) {
    return false;
  }
  int min_row, min_col;
  std::vector<size_t> matched_row_indices, matched_col_indices;
  Eigen::VectorXf col_vec = Eigen::MatrixXf::Constant(
    cost_matrix.rows(), 1, std::numeric_limits<float>::max());
  Eigen::RowVectorXf row_vec = Eigen::MatrixXf::Constant(
    1, cost_matrix.cols(), std::numeric_limits<float>::max());

  while (cost_matrix.minCoeff(&min_row, &min_col) <= matching_gate) {
    if (min_row >= cost_matrix.rows() || min_col >= cost_matrix.cols()) {
      break;
    }
    matched_pairs[min_row] = min_col;

    cost_matrix.row(min_row) = row_vec;
    cost_matrix.col(min_col) = col_vec;
    matched_row_indices.push_back(min_row);
    matched_col_indices.push_back(min_col);
  }

  std::vector<size_t> all_row_indices(cost_matrix.rows());
  std::iota(std::begin(all_row_indices), std::end(all_row_indices), 0);

  std::vector<size_t> all_col_indices(cost_matrix.cols());
  std::iota(std::begin(all_col_indices), std::end(all_col_indices), 0);

  std::sort(matched_col_indices.begin(), matched_col_indices.end());
  std::sort(matched_row_indices.begin(), matched_row_indices.end());

  std::set_difference(
    all_row_indices.begin(), all_row_indices.end(), matched_row_indices.begin(),
    matched_row_indices.end(),
    std::inserter(orphan_row_items, orphan_row_items.begin()));

  std::set_difference(
    all_col_indices.begin(), all_col_indices.end(), matched_col_indices.begin(),
    matched_col_indices.end(),
    std::inserter(orphan_col_items, orphan_col_items.begin()));
  return true;
}

bool YoloDerocatestage::StagePostProcess(const contextPtr &conext_ptr,
                                         const ProcessContextMap &contextMap) {
  REPORT_ERROR_IF_NULL(conext_ptr);
  TimeWatch time_watch;
  time_watch.start();
  auto outputs_vecs = conext_ptr->out_dataflow_;
  if (outputs_vecs.size() != 1 || outputs_vecs[0].size() != kTotalOut * sizeof(float)) {
    LOG(ERROR) << "YoloDerocatestage get error input!";
    return false;
  }
  std::vector<float> output;
  std::vector<std::vector<YoloV5BodyInfo>> nms_boxes_vec;
  float *inputarray[kOutCnt];
  inputarray[0] = reinterpret_cast<float *>(outputs_vecs[0].data());
  inputarray[1] = inputarray[0] + 3 * 4032 * kOutUnit;
  inputarray[2] = inputarray[0] + 3 * (4032 + 1008) * kOutUnit;
  DecodingOutput(inputarray, &output);
  ObtainBoxAndScore(output, &nms_boxes_vec);
  FilterBoxByScore(&nms_boxes_vec);
  auto associate_vec = ComputeAssociate(&nms_boxes_vec);
  conext_ptr->batches = associate_vec->size();
  conext_ptr->out_dataflow_.clear();
  conext_ptr->out_shape_.clear();
  std::transform(associate_vec->begin(), associate_vec->end(),
                 std::back_inserter(conext_ptr->out_dataflow_),
                 [&](PersonInfo &info) {
                   std::vector<char> flow;
                   std::vector<uint32_t> shape;
                   shape.push_back(sizeof(PersonInfo));
                   conext_ptr->out_shape_.push_back(shape);
                   flow.resize(sizeof(PersonInfo));
                   std::memcpy(flow.data(), &info, sizeof(PersonInfo));
                   return flow;
                 });
  DLOG(INFO) << "Run Yolo Output size is :" << associate_vec->size();
  LOG(INFO) << "Run Yolo PostDecorate stage run success, cost"
            << time_watch.stop() << "ms";
  return true;
}
REG_Stage(yolo, std::make_shared<YoloDerocatestage>())
}  // namespace pipeline