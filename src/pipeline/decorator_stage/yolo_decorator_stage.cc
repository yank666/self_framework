//
// Created by yankai.yan on 2021/4/15.
//

#include "yolo_decorator_stage.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "glog/logging.h"
#include "src/utils/base.h"
#include "src/pipeline/stage_register.h"

namespace pipeline {

const float kThreshold = 0.6f;

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

enum YoloDimDesc : int { idx_batch = 0, idx_channel, idx_h, idx_w };

enum ClassifyLabel : int {
  cls_person = 0,
  cls_shoulder,
  cls_face,
  cls_handpackage
};

inline float sigmoid(float x) { return (1 / (1 + exp(-x))); }

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
  return a.score > b.score;
}

bool YoloDerocatestage::StagePreProcess(const contextPtr &conext_ptr) {
  REPORT_ERROR_IF_NULL(conext_ptr);
  TImeWatch time_watch;
  time_watch.start();
  MakeGridNp();

  LOG(INFO) << "Run Yolo predecorate stage run success, cost"
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

          conf = *(inputarray[i] + idx + kConfiIdx) >= 0.3;

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
    body_info.score = score_max;
    body_info.type = max_score_idx - 1;
    body_info.box.x1 = output[idx * kOutActualShape[0][3] + 0] -
                       output[idx * kOutActualShape[0][3] + 2] / 2;
    body_info.box.y1 = output[idx * kOutActualShape[0][3] + 1] -
                       output[idx * kOutActualShape[0][3] + 3] / 2;
    body_info.box.x2 =
      body_info.box.x1 + output[idx * kOutActualShape[0][3] + 2];
    body_info.box.y2 =
      body_info.box.y1 + output[idx * kOutActualShape[0][3] + 3];

    body_info.related_score =
      output[idx * kOutActualShape[0][3] + kClassifyNum + 5];
    body_info.related_box.x1 =
      output[idx * kOutActualShape[0][3] + kClassifyNum + 5 + 1] -
      output[idx * kOutActualShape[0][3] + kClassifyNum + 5 + 3] / 2;
    body_info.related_box.y1 =
      output[idx * kOutActualShape[0][3] + kClassifyNum + 5 + 2] -
      output[idx * kOutActualShape[0][3] + kClassifyNum + 5 + 4] / 2;
    body_info.related_box.x2 =
      body_info.related_box.x1 +
      output[idx * kOutActualShape[0][3] + kClassifyNum + 5 + 3];
    body_info.related_box.y2 =
      body_info.related_box.y1 +
      output[idx * kOutActualShape[0][3] + kClassifyNum + 5 + 4];

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

    vec_boxes[max_score_idx - 1].push_back(body_info);
  }

  nms_boxes_vec->resize(kClassifyNum);
  for (size_t i = 0; i < kClassifyNum; ++i) {
    NMSProcess(vec_boxes[i], &(nms_boxes_vec->at(i)));
  }
}

void YoloDerocatestage::NMSProcess(const std::vector<YoloV5BodyInfo> &vec_boxes,
                                   std::vector<YoloV5BodyInfo> *bboxes_nms) {
  if (kThreshold < 0.0f) {
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
          kThreshold) {
        mask_merged[i] = 1;
      }
    }
  }
}

bool YoloDerocatestage::StagePostProcess(const contextPtr &conext_ptr) {
  REPORT_ERROR_IF_NULL(conext_ptr);
  TImeWatch time_watch;
  time_watch.start();
  auto outputs_vecs = conext_ptr->out_dataflow_;
  auto outputs_sizes = conext_ptr->out_shape_;

  std::vector<float> output;
  std::vector<std::vector<YoloV5BodyInfo>> nms_boxes_vec;
  float *inputarray[kOutCnt];
  inputarray[0] = outputs_vecs[0].data();
  inputarray[1] = outputs_vecs[0].data() + 3 * 4032 * kOutUnit;
  inputarray[2] = outputs_vecs[0].data() + 3 * (4032 + 1008) * kOutUnit;
  DecodingOutput(inputarray, &output);
  ObtainBoxAndScore(output, &nms_boxes_vec);
  LOG(INFO) << "outputs_vecs size is : " << outputs_vecs.size() << " "
            << outputs_vecs[0].size();
  LOG(INFO) << "outputs_sizes is:" << nms_boxes_vec.size();
  LOG(INFO) << "Run Yolo postdecorate stage run success, cost"
            << time_watch.stop() << "ms";
  return true;
}
REG_Stage(yolo, std::make_shared<YoloDerocatestage>())
}  // namespace pipeline