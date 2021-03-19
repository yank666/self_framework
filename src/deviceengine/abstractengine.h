//
// Created by yankai.yan on 2021/3/18.
//

#ifndef SELF_ARCHITECTURE_ABSTRACTENGINE_H
#define SELF_ARCHITECTURE_ABSTRACTENGINE_H
#include <string>
#include "../parse/parse_config.h"
namespace device {
using ModelCfgPtr = pipeline::ModelCfgPtr;
class AbstractEngine {
public:
  AbstractEngine(ModelCfgPtr model_cfg_ptr)
      : model_cfg_(model_cfg_ptr){};
  virtual ~AbstractEngine() = default;
  virtual int CreateGraph() = 0;
  virtual int RunProcess() = 0;

protected:
  pipeline::ModelCfgPtr model_cfg_;
};
}
#endif // SELF_ARCHITECTURE_ABSTRACTENGINE_H
