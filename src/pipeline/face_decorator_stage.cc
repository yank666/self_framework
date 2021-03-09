#include "face_decorator_stage.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include "register_stage.h"
namespace pipeline {
bool FaceDerocatestage::StagePreProcess(char *input_array) {
  std::cout << " Run FaceDerocatestage pre" << std::endl;
  return true;
}

bool FaceDerocatestage::StagePostProcess() {
  std::cout << " Run FaceDerocatestage post" << std::endl;
  return true;
  
}
REG_INFERENCE(face, std::make_shared<FaceDerocatestage>());
}