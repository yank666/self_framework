//
// Created by yankai on 2021/2/8.
//

#ifndef SELF_ARCHITECTURE_YOLOV5_DECODING_H
#define SELF_ARCHITECTURE_YOLOV5_DECODING_H


#include <vector>
#include <string>
#ifdef WithEIGEN
#include "Eigen/Core"
#include "unsupported/Eigen/CXX11/Tensor"
#endif
namespace decoding {
struct YoloV5Box {
  float x1;
  float y1;
  float x2;
  float y2;
};

struct YoloV5BodyInfo {
  float score;
  int type;
  YoloV5Box box;
};


class YoloV5Decode {
public:
  YoloV5Decode() = default;
  ~YoloV5Decode() = default;
  void MakeGridNp();
  void DecodingOutput(float **inputarray, std::vector<float> *output);
  void ObtainBoxAndScore(const std::vector<float> &output, std::vector<std::vector<YoloV5BodyInfo>> *nms_boxes_vec);
  void NMSProcess(const std::vector<YoloV5BodyInfo> &vec_boxes, std::vector<YoloV5BodyInfo> *bboxes_nms);
  void FillOutPut(std::vector<float> *out_array, const YoloV5BodyInfo& body);
  // std::vector<float> scores_;
  // std::vector<float> bboxs_;
  
  std::vector<std::vector<int32_t>> anchor_grid_;
  std::vector<std::vector<int32_t>> anchor_grid_np_;

#ifdef WithEIGEN
  void ForwordInferenceWithEigen(float **inputarray);
  std::vector<Eigen::TensorMap<Eigen::Tensor<int32_t, 4, Eigen::RowMajor>>> anchor_grid_tensor;
  std::vector<Eigen::TensorMap<Eigen::Tensor<int32_t, 4, Eigen::RowMajor>>> grid_tensor;
#endif

};
}

#endif //SELF_ARCHITECTURE_YOLOV5_DECODING_H
