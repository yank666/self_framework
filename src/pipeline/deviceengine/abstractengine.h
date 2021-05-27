//
// Created by yankai.yan on 2021/3/18.
//

#ifndef SELF_ARCHITECTURE_ABSTRACTENGINE_H
#define SELF_ARCHITECTURE_ABSTRACTENGINE_H
#include "src/parse/parse_config.h"
#include <string>
#include <functional>
#include "opencv2/opencv.hpp"

namespace device {
using ModelCfgPtr = pipeline::ModelCfgPtr;

class Context {
 public:
  Context() = default;
  ~Context() = default;
  std::string name_;
  uint32_t input_w;
  uint32_t input_h;
  uint32_t batches;
  std::string stream_id_;
  std::string equipment_id_;
  long long time_stamp_;
  uint32_t frame_num_;
  std::vector<std::vector<char>> ori_data;
  std::vector<std::vector<char>> dataflow_;
  std::vector<std::vector<uint32_t>> in_shape_;
  std::vector<std::vector<char>> out_dataflow_;
  std::vector<std::vector<uint32_t>> out_shape_;
  std::function<int(const char *strResult, int nResultLen, const char *StreamId,
                    int nStreamIdlen)>
    callback_function_;

  void TransmitContext(const std::shared_ptr<Context> dst_contest);
  void CopyContext(const std::shared_ptr<Context> dst_contest);
};
using contextPtr = std::shared_ptr<Context>;

class AbstractEngine {
 public:
  AbstractEngine(ModelCfgPtr model_cfg_ptr) : model_cfg_(model_cfg_ptr){};
  virtual ~AbstractEngine() = default;
  virtual int CreateGraph(const contextPtr &cur_context_ptr) = 0;
  virtual int RunProcess(const contextPtr &cur_context_ptr) = 0;
  bool SetModelCfg(ModelCfgPtr model_cfg_ptr);

 protected:
  pipeline::ModelCfgPtr model_cfg_;
};
}  // namespace device
#endif  // SELF_ARCHITECTURE_ABSTRACTENGINE_H
