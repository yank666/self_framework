//
// Created by yankai on 2021/3/8.
//
#include "fcos_decorator_stage.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "src/pipeline/stage_register.h"
namespace pipeline {
bool FcosDerocatestage::StagePreProcess(const contextPtr &conext_ptr,
                                        const ProcessContextMap &contextMap) {
  std::cout << " Run Fcos Derocatestage pre" << std::endl;
  return true;
}

bool FcosDerocatestage::StagePostProcess(const contextPtr &conext_ptr,
                                         const ProcessContextMap &contextMap) {
  std::cout << " Run Fcos Derocatestage post" << std::endl;
  return true;
}
REG_Stage(fcos, std::make_shared<FcosDerocatestage>())
}  // namespace pipeline