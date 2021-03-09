//
// Created by yankai on 2021/3/8.
//
#include "fcos_decorator_stage.h"
#include <vector>
#include <unordered_map>
#include <string>
#include "register_stage.h"
namespace pipeline {
bool FcosDerocatestage::StagePreProcess(char *input_array) {
  return true;
}

bool FcosDerocatestage::StagePostProcess() {
  return true;

}
REG_INFERENCE(fcos, std::make_shared<FcosDerocatestage>());
}