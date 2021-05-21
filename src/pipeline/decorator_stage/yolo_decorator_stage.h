//
// Created by yankai.yan on 2021/4/15.
//

#ifndef SELF_ARCHITECTURE_YOLO_DECORATOR_STAGE_H
#define SELF_ARCHITECTURE_YOLO_DECORATOR_STAGE_H

#include "src/pipeline/pipeline.h"
#include "Eigen/Dense"

namespace pipeline {
struct YoloV5Box {
  float x1;
  float y1;
  float x2;
  float y2;
  float conf;
};

struct YoloV5BodyInfo {
  float score;
  int type;
  YoloV5Box box;
  YoloV5Box related_box;
  float related_score;
  std::vector<std::pair<int, float>> attrs;  // 属性label: 属性score
};

struct PersonInfo {
  int32_t label;
  YoloV5Box body;
  YoloV5Box head;
  YoloV5Box face;

  std::vector<YoloV5Box> hand_packages;
  std::vector<std::pair<int, float>> attrs;
  std::vector<float> feature;
  bool face_observeed = false;
  bool head_observed = false;
};

class YoloDerocatestage : public DecoratorStage {
 public:
  YoloDerocatestage() : DecoratorStage() {}
  ~YoloDerocatestage() = default;

 protected:
  bool StagePreProcess(const contextPtr &conext_ptr,
                       const ProcessContextMap &contextMap);
  bool StagePostProcess(const contextPtr &conext_ptr,
                        const ProcessContextMap &contextMap);

 private:
  void MakeGridNp();
  void DecodingOutput(float **inputarray, std::vector<float> *output);
  void ObtainBoxAndScore(
    const std::vector<float> &output,
    std::vector<std::vector<YoloV5BodyInfo>> *nms_boxes_vec);
  void NMSProcess(const std::vector<YoloV5BodyInfo> &vec_boxes,
                  std::vector<YoloV5BodyInfo> *bboxes_nms);
  std::shared_ptr<std::vector<PersonInfo>> ComputeAssociate(
    std::vector<std::vector<YoloV5BodyInfo>> *nms_boxes);
  bool FindAssociationPairs(Eigen::MatrixXf &cost_matrix, float matching_gate,
                            std::unordered_map<size_t, size_t> &matched_pairs,
                            std::vector<size_t> &orphan_row_items,
                            std::vector<size_t> &orphan_col_items);
  void FilterBoxByScore(std::vector<std::vector<YoloV5BodyInfo>> *nms_boxes_vec);
  std::shared_ptr<std::vector<std::vector<int32_t>>> anchor_grid_{nullptr};
  std::shared_ptr<std::vector<std::vector<int32_t>>> anchor_grid_np_{nullptr};
};
}  // namespace pipeline

#endif  // SELF_ARCHITECTURE_YOLO_DECORATOR_STAGE_H
