

#include "parse_config.h"
#include <fstream>
#include <fcntl.h>
#include <string>
#include "glog/logging.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"
#include "google/protobuf/text_format.h"
#include "modules/proto/parser.pb.h"
#include "src/utils/errorcode.h"

using google_FileInputStream = google::protobuf::io::FileInputStream;
namespace pipeline {
constexpr char kIsEngine[] = "InferEngine";

int ParseConfig::ParseConfigFromProto(const std::string &cfg_file,
                                      pipeline::ModelCfgList *model_list) {
  int fd = open(cfg_file.c_str(), O_RDONLY);
  if (-1 == fd) {
    LOG(ERROR) << "Fail to open config file: " << cfg_file
               << ", please check file exist";
    return RET_INPUT_FILE_INVALID;
  }
  std::unique_ptr<google_FileInputStream> input_stream(
    new google_FileInputStream(fd));
  ParserModel::ModelParameter proto_params;
  bool success =
    google::protobuf::TextFormat::Parse(input_stream.get(), &proto_params);
  if (!success) {
    LOG(ERROR) << "Fail to parse config file";
    return RET_INPUT_FILE_INVALID;
  }
  close(fd);

  for (int i = 0; i < proto_params.infer_config_size(); ++i) {
    pipeline::ModelCfgPtr model_ptr = std::make_shared<pipeline::ModelConfig>();
    ParserModel::InferConfigParameter model_param =
      proto_params.infer_config(i);
    model_ptr->model_name_ = model_param.name();
    model_ptr->model_position_ = model_param.model_position();
    if (model_param.infer_config_type() != kIsEngine) {
      model_ptr->is_inferengine_ = false;
      model_list->emplace_back(model_ptr);
      continue;
    } else {
      model_ptr->is_inferengine_ = true;
    }
    model_ptr->model_binary_ = model_param.model_binary();
    model_ptr->model_type_ = model_param.infer_type();
    model_ptr->data_format_ = model_param.model_input_data_format();
    for (int i = 0; i < model_param.input_shape_size(); ++i) {
      ParserModel::BlobShape shape = model_param.input_shape(i);
      std::vector<uint32_t> inputs;
      for (int j = 0; j < shape.dim_size(); ++j) {
        inputs.push_back(shape.dim(j));
      }
      model_ptr->model_inshape_.push_back(inputs);
    }

    for (int i = 0; i < model_param.output_shape_size(); ++i) {
      ParserModel::BlobShape shape = model_param.output_shape(i);
      std::vector<uint32_t> outputs;
      for (int j = 0; j < shape.dim_size(); ++j) {
        outputs.emplace_back(shape.dim(j));
      }
      model_ptr->model_outshape_.emplace_back(outputs);
    }

    for (int i = 0; i < model_param.mean_param_size(); ++i) {
      ParserModel::meanParameter mean_param = model_param.mean_param(i);
      for (int j = 0; j < mean_param.mean_size(); ++j) {
        model_ptr->model_mean_.emplace_back(mean_param.mean(j));
      }
    }

    for (int i = 0; i < model_param.norm_param_size(); ++i) {
      ParserModel::normParameter norm_param = model_param.norm_param(i);
      for (int j = 0; j < norm_param.norm_size(); ++j) {
        model_ptr->model_norm_.emplace_back(norm_param.norm(j));
      }
    }
    model_list->emplace_back(model_ptr);
  }
  DLOG(INFO) << "Parse config file Success";
  return RET_OK;
}
};  // namespace pipeline