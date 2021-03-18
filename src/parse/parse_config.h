//
// Created by yankai.yan on 2021/3/11.
//

#ifndef SELF_ARCHITECTURE_PARSECONFIG_H
#define SELF_ARCHITECTURE_PARSECONFIG_H
#include <string>
#include "src/pipeline/pipeline.h"

namespace parse {
class ParseConfig {
public:
  ParseConfig() = default;
  ~ParseConfig() = default;
  static bool ParseConfigFromProto(const std::string &cfg_file,
                                   pipeline::ModelCfgList *model_list);
};
}
#endif // SELF_ARCHITECTURE_PARSECONFIG_H
