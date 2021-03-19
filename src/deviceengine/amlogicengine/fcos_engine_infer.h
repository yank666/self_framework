//
// Created by yankai.yan on 2021/3/18.
//

#ifndef SELF_ARCHITECTURE_FCOS_ENGINE_INFER_H
#define SELF_ARCHITECTURE_FCOS_ENGINE_INFER_H
#include "amlogical_engine_infer.h"
namespace device {
class FcosEngineInfer : public AmlogicEngine {
public:
  FcosEngineInfer();
  ~FcosEngineInfer();
  int CreateGraph();
  int RunProcess();
};
}
#endif // SELF_ARCHITECTURE_FCOS_ENGINE_INFER_H
