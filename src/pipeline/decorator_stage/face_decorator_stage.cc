#include "face_decorator_stage.h"
#include "src/pipeline/stage_register.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
namespace pipeline {
bool FaceDerocatestage::StagePreProcess(const contextPtr &conext_ptr) {
  std::cout << " Run FaceDerocatestage pre" << std::endl;
  return true;
}

bool FaceDerocatestage::StagePostProcess(const contextPtr &conext_ptr) {
  std::cout << " Run FaceDerocatestage post" << std::endl;
  return true;
}
REG_Stage(face, std::make_shared<FaceDerocatestage>())
}  // namespace pipeline