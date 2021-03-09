#include "yolov5_decoding.h"
#include <math.h>
#include <chrono>
#include <iostream>
#include <algorithm>
namespace decoding{
static const uint32_t kClassifyNum = 4;
static const float kThreshold = 0.6f;
static const uint32_t kYoloV5ImageWidth = 672;
static const uint32_t kYoloV5ImageHeight = 384;
static const uint32_t kOutPutNum = 3;
static const uint32_t kYoloV5AnchorNum = 6;
static const uint32_t kYoloV5Anchor[][kYoloV5AnchorNum] = {{65,188, 120,269, 189,486}, {22,60, 35,42, 42,106}, {5,5, 10,11, 18,22}};
static const uint32_t kYoloV5Strie[] = {32, 16, 8};
std::vector<std::vector<uint32_t>> kYoloV5Shape{{1,3,252,9}, {1,3,1008,9}, {1,3,4032,9}};
std::vector<std::vector<uint32_t>> kYoloV5ShapeNp{{1,3,252,2}, {1,3,1008,2}, {1,3,4032,2}};
static const uint32_t kConfiIdx = 4;
static const uint32_t kOutputUnit = 142884;
enum ClassifyLabel : int {cls_person = 0, cls_shoulder, cls_face, cls_handpackage};
enum DimIdx : int {idx_batch= 0, idx_channel = 1, idx_height = 2, idx_width = 3};

void YoloV5Decode::MakeGridNp() {
  if (anchor_grid_.size() != 0 && anchor_grid_np_.size() != 0) {
    return;
  }
  anchor_grid_.resize(kOutPutNum);
  anchor_grid_np_.resize(kOutPutNum);
  for (size_t i = 0; i < kOutPutNum; i++) {
    int32_t x_dims = kYoloV5ImageWidth / kYoloV5Strie[i];
    int32_t y_dims = kYoloV5ImageHeight / kYoloV5Strie[i];
    
    for (size_t x = 0;  x < y_dims; ++x) {
      for (size_t y = 0;  y < x_dims; ++y) {
        anchor_grid_[i].push_back(y);
        anchor_grid_[i].push_back(x);
      }
    }
    size_t  grid_size =  anchor_grid_[i].size();
    for (size_t k = 1; k < kOutPutNum; ++k) {
      anchor_grid_[i].insert(anchor_grid_[i].end(), anchor_grid_[i].begin(), anchor_grid_[i].begin() + grid_size);
    }
    
    for (size_t num = 0; num <  kOutPutNum; num++) {
      for (size_t idx = 0; idx < x_dims * y_dims; ++idx) {
        for (size_t count = 0; count < kYoloV5AnchorNum / kOutPutNum; count++) {
          anchor_grid_np_[i].push_back(kYoloV5Anchor[i][num * 2 + count]);
        }
      }
    }
  }
#ifdef WithEIGEN
  grid_tensor.emplace_back(anchor_grid_[0].data(), 1 ,3 , 252, 2);
  grid_tensor.emplace_back(anchor_grid_[1].data(), 1 ,3 , 1008, 2);
  grid_tensor.emplace_back(anchor_grid_[2].data(), 1 ,3 , 4032, 2);
  
  anchor_grid_tensor.emplace_back(anchor_grid_np_[0].data(), 1 ,3 , 252, 2);
  anchor_grid_tensor.emplace_back(anchor_grid_np_[1].data(), 1 ,3 , 1008, 2);
  anchor_grid_tensor.emplace_back(anchor_grid_np_[2].data(), 1 ,3 , 4032, 2);
#endif
}

void YoloV5Decode::DecodingOutput(float **inputarray, std::vector<float> *output) {
  auto start = std::chrono::system_clock::now();
  std::vector<std::vector<float>> output_y(kOutPutNum);
  std::vector<std::vector<int32_t>> output_grid_np;
  output_grid_np.resize(kOutPutNum);
  std::vector<std::vector<int32_t>> output_anchor_grid_np;
  output_anchor_grid_np.resize(kOutPutNum);
  bool conf = false;
  for (size_t i = 0; i < kOutPutNum; ++i) {
    for (size_t batch = 0; batch < kYoloV5Shape[i][0];  ++batch) {
      for (size_t channel = 0; channel < kYoloV5Shape[i][1];  ++channel) {
        for (size_t height = 0; height < kYoloV5Shape[i][2];  ++height) {
          uint32_t idx = height * kYoloV5Shape[i][3] + channel * kYoloV5Shape[i][2] * kYoloV5Shape[i][3] +
                         batch *kYoloV5Shape[i][1] * kYoloV5Shape[i][2] * kYoloV5Shape[i][3];
          conf = *(inputarray[i] + idx + kConfiIdx) >= 0.3;
          if (conf) {
            output_y[i].insert(output_y[i].end(),inputarray[i] + idx, inputarray[i] + idx +  kYoloV5Shape[i][3]);
            uint32_t idx_np = height * kYoloV5ShapeNp[i][3] + channel * kYoloV5ShapeNp[i][2] * kYoloV5ShapeNp[i][3]
                              + batch *kYoloV5ShapeNp[i][1] * kYoloV5ShapeNp[i][2] * kYoloV5ShapeNp[i][3];
            output_grid_np[i].insert(output_grid_np[i].end(),
                                     anchor_grid_[i].begin() + idx_np,anchor_grid_[i].begin() + idx_np + 2);
            output_anchor_grid_np[i].insert(output_anchor_grid_np[i].end(),
                                            anchor_grid_np_[i].begin() + idx_np,anchor_grid_np_[i].begin() + idx_np + 2);
          }
        }
      }
    }
  }
  
  for (size_t i = 0; i < kOutPutNum; ++i) {
    for (size_t idx = 0 ; idx * kYoloV5Shape[i][3] < output_y[i].size(); ++idx) {
      float temp = output_y[i][idx * kYoloV5Shape[i][3] + 0];
      float temp1 =  output_grid_np[i][idx * 2 + 0];
      output_y[i][idx * kYoloV5Shape[i][3] + 0] =
          (output_y[i][idx * kYoloV5Shape[i][3] + 0] * 2.0 - 0.5 + output_grid_np[i][idx * 2 + 0]) * kYoloV5Strie[i];
      output_y[i][idx * kYoloV5Shape[i][3] + 1] =
          (output_y[i][idx * kYoloV5Shape[i][3] + 1] * 2.0 - 0.5 + output_grid_np[i][idx * 2 + 1]) * kYoloV5Strie[i];
      output_y[i][idx * kYoloV5Shape[i][3] + 2] =
          std::pow(output_y[i][idx * kYoloV5Shape[i][3] + 2] *2.0, 2) * output_anchor_grid_np[i][idx * 2 + 0];
      output_y[i][idx * kYoloV5Shape[i][3] + 3] =
          std::pow(output_y[i][idx * kYoloV5Shape[i][3] + 3] *2.0, 2) * output_anchor_grid_np[i][idx * 2 + 1];
    }
    output->insert(output->end(), output_y[i].begin(), output_y[i].end());
  }
  auto end = std::chrono::system_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  std::cout <<  "without eigen cost:" <<  double(duration.count()) * std::chrono::microseconds::period::num  << "ms" << std::endl;
  return;
}

void YoloV5Decode::ObtainBoxAndScore(const std::vector<float> &output,
                                                  std::vector<std::vector<YoloV5BodyInfo>> *nms_boxes_vec) {
  std::vector<std::vector<YoloV5BodyInfo>> vec_boxes;
  vec_boxes.resize(kClassifyNum);
  for (size_t idx = 0 ; idx * kYoloV5Shape[0][3] < output.size(); ++idx) {
    int32_t max_score_idx = -1;
    float score_max = 0.0f;
    for (uint32_t score_idx = 1; score_idx < 5; ++score_idx) {
      float score_temp = output[idx * kYoloV5Shape[0][3] + kConfiIdx] * output[idx * kYoloV5Shape[0][3] + kConfiIdx + score_idx];
      if (score_temp >=score_max) {
        score_max = score_temp;
        max_score_idx = score_idx;
      }
    }
    YoloV5BodyInfo body_info;
    body_info.score = score_max;
    body_info.type = max_score_idx - 1;
    body_info.box.x1 = output[idx * kYoloV5Shape[0][3] + 0] - output[idx * kYoloV5Shape[0][3] + 2] / 2;
    body_info.box.y1 = output[idx * kYoloV5Shape[0][3] + 1] - output[idx * kYoloV5Shape[0][3] + 3] / 2;
    body_info.box.x2 = body_info.box.x1 + output[idx * kYoloV5Shape[0][3] + 2];
    body_info.box.y2 = body_info.box.y1 + output[idx * kYoloV5Shape[0][3] + 3];
    vec_boxes[max_score_idx - 1].push_back(body_info);
    //LOG(ERROR) << "==========box is: ============="<< body_info.box.x1 << " " << body_info.box.y1 << " "<< body_info.box.x2 << " "<< body_info.box.y2;
  }
  
  nms_boxes_vec->resize(kClassifyNum);
  for (size_t i = 0; i < kClassifyNum; ++i) {
    NMSProcess(vec_boxes[i], &(nms_boxes_vec->at(i)));
  }
}

static bool CompareBBox(const YoloV5BodyInfo &a, const YoloV5BodyInfo &b) {
  return a.score > b.score;
}

void YoloV5Decode::NMSProcess(const std::vector<YoloV5BodyInfo> &vec_boxes, std::vector<YoloV5BodyInfo> *bboxes_nms) {
  if (kThreshold < 0.0f) {
//    LOG(ERROR) << "threshold in NMSProcess is wrong.";
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

void YoloV5Decode::FillOutPut(std::vector<float> *out_array, const YoloV5BodyInfo& body) {
  out_array->push_back(body.type);
  out_array->push_back(body.score);
  out_array->push_back(body.box.x1);
  out_array->push_back(body.box.y1);
  out_array->push_back(body.box.x2);
  out_array->push_back(body.box.y2);
}


#ifdef WithEIGEN
void YoloV5Decode::ForwordInferenceWithEigen(float **inputarray) {
  auto start = std::chrono::system_clock::now();
  std::vector<uint32_t> shape1={1,3,252,9};
  if (inputarray[1] == nullptr) {
    return;
  }
  std::vector<Eigen::TensorMap<Eigen::Tensor<float, 4, Eigen::RowMajor>>> input_tensor;
  std::vector<Eigen::Tensor<float, 4, Eigen::RowMajor>> undecode_tensor_vec;
  input_tensor.reserve(kOutPutNum);
  input_tensor.emplace_back(inputarray[0], 1,3,252,9);
  input_tensor.emplace_back(inputarray[1], 1,3,1008,9);
  input_tensor.emplace_back(inputarray[2], 1,3,4032,9);


  for (size_t idx_t = 0; idx_t < input_tensor.size(); idx_t++) {
    for (int i = 0; i < input_tensor[idx_t].dimension(0); i++) {
      for (int j = 0; j < input_tensor[idx_t].dimension(1); j++) {
        for (int k = 0; k < input_tensor[idx_t].dimension(2); k++) {
          if (input_tensor[idx_t](i, j, k, 4) >= 0.3) {
            Eigen::array<long,4> offset = {i, j, k, 0};
            Eigen::array<long,4> extent_encode = { 1, 1, 1,9};
            Eigen::array<long,4> extent_grid = { 1, 1, 1, 2};
            Eigen::Tensor<float, 4, Eigen::RowMajor> undecode_tensor(input_tensor[idx_t].slice(offset, extent_encode));
            Eigen::Tensor<int32_t, 4, Eigen::RowMajor> output_grid(grid_tensor[idx_t].slice(offset, extent_grid));
            Eigen::Tensor<int32_t, 4, Eigen::RowMajor> output_anchor_grid(anchor_grid_tensor[idx_t].slice(offset, extent_grid));
            for (int i = 0; i < 2; i++) {
              undecode_tensor(0, 0, 0, i) = (undecode_tensor(0, 0, 0, i) * 2 - 0.5 + output_grid(0, 0, 0, i)) *
                  kYoloV5Strie[idx_t];
            }
            for (int i = 2; i < 4; i++) {
//              undecode_tensor(0, 0, 0, i) = pow(undecode_tensor(0, 0, 0, i) * 2, 2.0) * output_anchor_grid(0, 0 , 0,
                  i - 2);
            }
            undecode_tensor_vec.push_back(undecode_tensor);
          }
        }
      }
    }
  }

  for (auto &decode_tensor : undecode_tensor_vec) {
    for (int i = 5; i < decode_tensor.dimension(3); i++) {
      decode_tensor(0, 0, 0, i) = decode_tensor(0, 0, 0, i) * decode_tensor(0, 0, 0, 4);
    }
    decode_tensor(0, 0, 0, 0) = decode_tensor(0, 0, 0, 0) - decode_tensor(0, 0, 0, 2) / 2;
    decode_tensor(0, 0, 0, 1) = decode_tensor(0, 0, 0, 1) - decode_tensor(0, 0, 0, 3) / 2;
    decode_tensor(0, 0, 0, 2) = decode_tensor(0, 0, 0, 0) + decode_tensor(0, 0, 0, 2);
    decode_tensor(0, 0, 0, 3) = decode_tensor(0, 0, 0, 1) + decode_tensor(0, 0, 0, 3);
  }
  auto end = std::chrono::system_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  std::cout <<  "with eigen cost:" <<  double(duration.count()) * std::chrono::microseconds::period::num  << "ms" << std::endl;
  return;
}
#endif
}
