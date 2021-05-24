
#include "common_test.h"
#include <fstream>
#include "opencv2/opencv.hpp"
#include "glog/logging.h"
#include "gtest/gtest.h"
#include "src/pipeline/deviceengine/abstractengine.h"
#include "src/pipeline/decorator_stage/yolo_decorator_stage.h"
#include "src/pipeline/decorator_stage/upload_decorator_stage.h"
#include "thirdparty/tracker/TrackerLib_api_c.h"
#include "src/parse/parse_json.h"

using namespace pipeline;
using namespace device;

class Unit : public CommonTest  {
 public:
 protected:
 protected:
  virtual void TearDown() {}
  virtual void SetUp() {}

  Unit() = default;
  ~Unit() = default;
};

TEST_F(Unit, yolodecorator) {
  FLAGS_minloglevel = 0;
  char *in = nullptr;
  const uint32_t kInputSize = 333396;
  in = ReadFromFile("torch_out.bins", kInputSize * sizeof(float));
  ASSERT_NE(nullptr, in);
  contextPtr context_ptr = std::make_shared<Context>();
  std::vector<uint32_t> data_size_set = {kInputSize};
  context_ptr->out_dataflow_.resize(1);
  context_ptr->out_dataflow_[0].resize(kInputSize * sizeof(float));
  memcpy(context_ptr->out_dataflow_[0].data(), in, kInputSize * sizeof(float));
  auto yolo_stage_ptr = std::make_shared<YoloDerocatestage>();
  ProcessContextMap process_context_;
  yolo_stage_ptr->RunStage(context_ptr, process_context_);
  free(in);
  LOG(INFO) << "Run SUCCESS!";
}

TEST_F(Unit, parseconfig) {
  uint32_t kInputUnit = 1 * 3 * 384 * 672;
  cv::Mat img = cv::imread("/home/yankai.yan/workbase/codeLib/refactor/tests/bin/input.jpg");
  ASSERT_NE(img.empty(), true);
  std::string model_cfg_file = "/home/yankai.yan/workbase/codeLib/refactor/modules/models.cfg";
  std::vector<pipeline::ModelCfgPtr> cfg_vec;
  int ret = ParseConfig::ParseConfigFromProto(model_cfg_file, &cfg_vec);
  ASSERT_EQ(0, ret);
  LOG(INFO) << "Run SUCCESS!";
}

TEST_F(Unit, tracker) {
  cv::Mat img = cv::imread("/home/yankai.yan/workbase/codeLib/refactor/tests/bin/input.jpg");
  ASSERT_NE(img.empty(), true);
  detect_res body_det = {0};
  detect_res body_track = {0};
  body_det.detect_res_info[body_det.num_detections].track_id = -1;
  body_det.detect_res_info[body_det.num_detections].label = 0;
  body_det.detect_res_info[body_det.num_detections].prob = 0.852488;
  body_det.detect_res_info[body_det.num_detections].box.x1 = 265.979919;
  body_det.detect_res_info[body_det.num_detections].box.y1 = 60.7324295;
  body_det.detect_res_info[body_det.num_detections].box.x2 = 315.11496;
  body_det.detect_res_info[body_det.num_detections].box.y2 = 188.153015;
  body_det.num_detections = 1;
  body_track.num_detections = 0;
  void* tracker_api = tracker_init_with_config("/home/yankai.yan/workbase/codeLib/refactor/modules/config_file/tracker_config.json");
  tracker_inference(&body_det, &body_track, 0, img.data, img.cols, img.rows, 0, tracker_api);
  ASSERT_EQ(1, body_track.num_detections);
  tracker_release(tracker_api);
}

TEST_F(Unit, upload) {
  cv::Mat img = cv::imread("/home/yankai.yan/workbase/codeLib/refactor/tests/bin/input.jpg");
  ASSERT_NE(img.empty(), true);
  detect_res body_det = {0};
  detect_res body_track = {0};
  body_det.detect_res_info[body_det.num_detections].track_id = -1;
  body_det.detect_res_info[body_det.num_detections].label = 0;
  body_det.detect_res_info[body_det.num_detections].prob = 0.852488;
  body_det.detect_res_info[body_det.num_detections].box.x1 = 265.979919;
  body_det.detect_res_info[body_det.num_detections].box.y1 = 60.7324295;
  body_det.detect_res_info[body_det.num_detections].box.x2 = 315.11496;
  body_det.detect_res_info[body_det.num_detections].box.y2 = 188.153015;
  body_det.num_detections = 1;
  body_track.num_detections = 0;
  void* tracker_api = tracker_init_with_config("/home/yankai.yan/workbase/codeLib/refactor/modules/config_file/tracker_config.json");
  tracker_inference(&body_det, &body_track, 0, img.data, img.cols, img.rows, 0, tracker_api);
  ASSERT_EQ(1, body_track.num_detections);

  UploadDerocatestage op;
  contextPtr context_ptr = std::make_shared<Context>();
  context_ptr->out_dataflow_.resize(1);
  context_ptr->out_dataflow_[0].resize(sizeof(detect_res));
  memcpy(context_ptr->out_dataflow_[0].data(), &body_track, sizeof(body_track));
  ProcessContextMap process_context_;
  op.RunStage(context_ptr, process_context_);

  tracker_release(tracker_api);
  LOG(INFO) << "Run SUCCESS!";
}

TEST_F(Unit, parsejson) {
  std::string init_param = "{\"nGpuId\":0,\"fps\":25}";
  std::string streamInfo = "{\"streamId\":\"1022\",\"equipmentId\":\"1022\",\"stream_width\":1280,\"stream_height\":720,\"taskType\":\"realVideoStream\",\"config\":{\"Graphs\":[{\"areaId\":\"1\",\"shapeType\":\"checkOut\",\"shapeList\":[{\"x\":\"0.0\",\"y\":\"0.0\"},{\"x\":\"0.0\",\"y\":\"1.0\"},{\"x\":\"1.0\",\"y\":\"1.0\"},{\"x\":\"1.0\",\"y\":\"0.0\"}],\"algoType\":\"regionAttribute\"}]}}";
  auto stream_info = std::make_shared<StreamInfo>();
  auto ret = ParseJson::ParseVideoJson(const_cast<char *>(streamInfo.data()), stream_info);
  ASSERT_EQ(0, ret);
  LOG(INFO) << "Run SUCCESS!";
}