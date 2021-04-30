//
// Created by yankai.yan on 2021/4/28.
//

#include "parse_video.h"
#include "glog/logging.h"
#include "opencv2/opencv.hpp"
#include "src/utils/errorcode.h"


namespace pipeline {

int ParseVideo::ParseVideoFromStream(const std::string stream_file,
                         int32_t streamd_id) {
  if (stream_file.empty()) {
    return RET_INPUT_FILE_INVALID;
  }
  cv::VideoCapture capture(stream_file);
  if (!capture.isOpened())
  {
    LOG(ERROR) << "Parse video from stream file, open " << stream_file << "failed !";
    return RET_INPUT_PARAM_INVALID;
  }
  stream_fps_ = capture.get(CV_CAP_PROP_FPS);
  stream_frames_= capture.get(CV_CAP_PROP_FRAME_COUNT);
  stream_width_ = capture.get(cv::CAP_PROP_FRAME_WIDTH);
  stream_height_ = capture.get(cv::CAP_PROP_FRAME_HEIGHT);

  while(!stop_) {

  }

}
}