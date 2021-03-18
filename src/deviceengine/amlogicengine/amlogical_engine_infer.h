//
// Created by yankai.yan on 2021/3/18.
//

#ifndef SELF_ARCHITECTURE_AMLOGICAL_ENGINE_INFER_H
#define SELF_ARCHITECTURE_AMLOGICAL_ENGINE_INFER_H

#include <string>

namespace device {
struct InferenceTensor {};
class AmlogicEngine {
public:
  AmlogicEngine(std::string name_engine) : name_(name_engine) {}
  virtual ~AmlogicEngine();
  virtual int CreateGraph(const std::string &binary_path);
  virtual int RunProcess(InferenceTensor &inputs);

protected:
  //  vsi_nn_graph_t *self_graph_ = NULL;
  std::string name_;
};
}
#endif // SELF_ARCHITECTURE_AMLOGICAL_ENGINE_INFER_H
