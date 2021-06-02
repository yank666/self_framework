//
// Created by yankai.yan on 2021/4/28.
//

#ifndef SELF_ARCHITECTURE_PARSE_VIDEO_H
#define SELF_ARCHITECTURE_PARSE_VIDEO_H

#include <memory>
#include <string>
#include <list>
#include <mutex>
#include <atomic>
#include "opencv2/opencv.hpp"
#include "src/utils/circlebuffer.h"

namespace pipeline {
class ParseVideo {
 public:
  explicit ParseVideo(const uint32_t buffer_size = 10);
  ~ParseVideo() = default;
  int ParseVideoFromStream(const std::string stream_file, int32_t streamd_id);
  int ParseVideoByFFmpeg(const std::string stream_file);
  bool GetParseDate(cv::Mat &result);
  uint32_t GetVideoFrames() { return frame_list_->size(); };
  bool GetReady() { return be_ready_; };
  uint32_t GetTotalFrames() { return stream_frames_; }
  std::atomic<bool> stop_;

 protected:
  double stream_fps_;
  uint32_t stream_frames_;
  uint32_t stream_width_;
  uint32_t stream_height_;
  uint32_t max_;
  //   CircleBuffer<cv::Mat> frame_list_;
  std::shared_ptr<CircleBuffer<cv::Mat>> frame_list_;
  bool be_ready_{false};

 private:
  cv::Mat ret_;
  std::mutex task_mutex_;
};

}  // namespace pipeline
#endif  // SELF_ARCHITECTURE_PARSE_VIDEO_H
