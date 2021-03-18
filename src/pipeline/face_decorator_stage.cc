#include "face_decorator_stage.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include "register_stage.h"
namespace pipeline {
bool FaceDerocatestage::StagePreProcess(const ProcessContextMap &conext_map) {
  std::cout << " Run FaceDerocatestage pre" << std::endl;
  return true;
}

bool FaceDerocatestage::StagePostProcess(const ProcessContextMap &conext_map) {
  std::cout << " Run FaceDerocatestage post" << std::endl;
  return true;
  
}
REG_Stage(face, std::make_shared<FaceDerocatestage>());
}