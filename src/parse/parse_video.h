//
// Created by yankai.yan on 2021/4/28.
//

#ifndef SELF_ARCHITECTURE_PARSE_VIDEO_H
#define SELF_ARCHITECTURE_PARSE_VIDEO_H

#include <string>
namespace pipeline {
class ParseVideo {
 public:
  ParseVideo() = default;
  ~ParseVideo() = default;
  int ParseVideoFromStream(const std::string stream_file,
                                  int32_t streamd_id);

 protected:
   double stream_fps_;
   uint32_t stream_frames_;
   uint32_t stream_width_;
   uint32_t stream_height_;
   bool stop_;
};
}
#endif  // SELF_ARCHITECTURE_PARSE_VIDEO_H
