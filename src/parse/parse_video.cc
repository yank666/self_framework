//
// Created by yankai.yan on 2021/4/28.
//

#include "parse_video.h"
#include <unistd.h>
#include "glog/logging.h"
#include "opencv2/opencv.hpp"
#include "src/utils/errorcode.h"
#include "src/utils/base.h"

namespace pipeline {
constexpr uint32_t kMaxBufferSize = 20;
constexpr uint32_t kMillisecondUnit = 1000;

ParseVideo::ParseVideo(const uint32_t buffer_size) {
  max_ = (buffer_size > kMaxBufferSize) ? kMaxBufferSize : buffer_size;
  stop_ = false;
}

int ParseVideo::ParseVideoFromStream(const std::string stream_file,
                                     int32_t streamd_id) {
  if (stream_file.empty()) {
    stop_ = true;
    be_ready_ = false;
    return RET_INPUT_FILE_INVALID;
  }

  cv::VideoCapture capture(stream_file, -1);
  if (!capture.isOpened()) {
    LOG(ERROR) << "Parse video failed, open " << stream_file << "failed!";
    stop_ = true;
    be_ready_ = false;
    return RET_INPUT_PARAM_INVALID;
  }
  frame_list_ = std::make_shared<CircleBuffer<cv::Mat>>(max_);
  REPORT_EXCEPTION_IF_NULL(frame_list_);

  stream_frames_ = capture.get(CV_CAP_PROP_FRAME_COUNT);
  stream_width_ = capture.get(cv::CAP_PROP_FRAME_WIDTH);
  stream_height_ = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
  cv::Mat frame;
  uint32_t total_frames = 0;
  while (!stop_ && total_frames < stream_frames_) {
    capture >> frame;
    if (frame.empty()) {
      LOG(INFO) << "Get frame from video got empty, total obtain frame : "
                << total_frames;
      be_ready_ = false;
      break;
    }
    while (frame_list_->size() >= max_) {
      be_ready_ = true;
      usleep(50 * kMillisecondUnit);
    }
    frame_list_->push(frame);
    total_frames++;
    DLOG(INFO) << "frame_list_ push " << frame_list_->size() << " "
               << total_frames;
  }
  stop_ = true;
  be_ready_ = false;
  return RET_OK;
}

bool ParseVideo::GetParseDate(cv::Mat &result) {
  if (frame_list_->size() == 0) {
    return false;
  }
  bool ret = frame_list_->pop(result);
  return ret;
};
}  // namespace pipeline