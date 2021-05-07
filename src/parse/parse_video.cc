//
// Created by yankai.yan on 2021/4/28.
//

#include "parse_video.h"
#include <unistd.h>
#include "src/utils/errorcode.h"
#include "glog/logging.h"
#include "opencv2/opencv.hpp"
#include "src/utils/errorcode.h"


namespace pipeline {
constexpr uint32_t  kMaxBufferSize = 10;
constexpr uint32_t  kMillisecondUnit = 1000;

int ParseVideo::ParseVideoFromStream(const std::string stream_file,
                         int32_t streamd_id) {
  if (stream_file.empty()) {
    return RET_INPUT_FILE_INVALID;
  }
  cv::VideoCapture capture(stream_file);
  if (!capture.isOpened())
  {
    LOG(ERROR) << "Parse video failed, open " << stream_file << "failed!";
    return RET_INPUT_PARAM_INVALID;
  }

  stream_frames_= capture.get(CV_CAP_PROP_FRAME_COUNT);
  stream_width_ = capture.get(cv::CAP_PROP_FRAME_WIDTH);
  stream_height_ = capture.get(cv::CAP_PROP_FRAME_HEIGHT);

  max_ = kMaxBufferSize;
  cv::Mat frame;
  uint32_t total_frames = 0;
  while(!stop_ && total_frames < 200) {
    capture >> frame;
    if (frame.empty()) {
      LOG(INFO) << "Get frame from video got empty, total obtain frame : " << total_frames;
      break;
    }
    while (frame_list_.size() >= max_) {
      be_ready_ = true;
      usleep(kMillisecondUnit);
      LOG(INFO) << "sleep  kMillisecondUnit!!!!!!!!!" << total_frames ;
    }

    frame_list_.push(frame);
    total_frames++;

    LOG(INFO) << "frame_list_ push " << frame_list_.size();

  }
}

bool ParseVideo::GetParseDate(cv::Mat &result) {
  if (frame_list_.size() == 0) {
    return false;
  }
  ret_ = frame_list_.pop(result);
  return true;
};

void  ParseVideo::SetParseBufferSize(const uint32_t max_num) {
   max_ = (max_num > kMaxBufferSize) ? kMaxBufferSize : max_num;
}

uint32_t ParseVideo::GetVideoFrames() {
  return frame_list_.size();
}

bool ParseVideo::GetReady() {
  return be_ready_;
}
}