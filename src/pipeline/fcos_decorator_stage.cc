//
// Created by yankai on 2021/3/8.
//
#include "fcos_decorator_stage.h"
#include "stage_register.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
namespace pipeline {
bool FcosDerocatestage::StagePreProcess(const ProcessContextMap &conext_map) {
  std::cout << " Run Fcos Derocatestage pre" << std::endl;
  return true;
}

bool FcosDerocatestage::StagePostProcess(const ProcessContextMap &conext_map) {
  std::cout << " Run Fcos Derocatestage post" << std::endl;
  return true;

}
REG_Stage(fcos, std::make_shared<FcosDerocatestage>());
}