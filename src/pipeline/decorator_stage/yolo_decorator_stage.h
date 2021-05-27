//
// Created by yankai.yan on 2021/4/15.
//

#ifndef SELF_ARCHITECTURE_YOLO_DECORATOR_STAGE_H
#define SELF_ARCHITECTURE_YOLO_DECORATOR_STAGE_H

#include "src/pipeline/pipeline.h"
#include "Eigen/Dense"
#include "src/utils/common_struct.h"
namespace pipeline {

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
  void FilterBoxByScore(
    std::vector<std::vector<YoloV5BodyInfo>> *nms_boxes_vec);
  std::shared_ptr<std::vector<std::vector<int32_t>>> anchor_grid_{nullptr};
  std::shared_ptr<std::vector<std::vector<int32_t>>> anchor_grid_np_{nullptr};
};
}  // namespace pipeline

#endif  // SELF_ARCHITECTURE_YOLO_DECORATOR_STAGE_H
