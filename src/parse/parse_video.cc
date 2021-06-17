//
// Created by yankai.yan on 2021/4/28.
//

#include "parse_video.h"
#include <unistd.h>
#include "glog/logging.h"
#include "opencv2/opencv.hpp"
#include "src/utils/errorcode.h"
#include "src/utils/base.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "libavutil/avutil.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"

#ifdef __cplusplus
}
#endif

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

  cv::VideoCapture capture(stream_file);
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

int ParseVideo::ParseVideoByFFmpeg(const std::string stream_file) {
  AVFormatContext *pFormatCtx;
  int videoindex;
  AVCodecContext *pCodecCtx;
  AVCodec *pCodec;
  AVFrame *pFrame, *pFrameRGB;
  uint8_t *out_buffer;
  AVPacket *packet;
  int ret;
  struct SwsContext *img_convert_ctx;
  //加载socket库以及网络加密协议相关的库，为后续使用网络相关提供支持
  avformat_network_init();
  //初始化AVFormatContext  结构
  pFormatCtx = avformat_alloc_context();
  //打开音视频文件并初始化AVFormatContext结构体
  ret = avformat_open_input(&pFormatCtx, stream_file.c_str(), NULL, NULL);
  if (ret != 0) {
    stop_ = true;
    be_ready_ = false;
    return ret;
  }

  //根据AVFormatContext结构体,来获取视频上下文信息,并初始化streams[]成员
  ret = avformat_find_stream_info(pFormatCtx, NULL);
  if (ret != 0) {
    stop_ = true;
    be_ready_ = false;
    return ret;
  }

  videoindex = -1;
  //根据type参数从ic->
  // streams[]里获取用户要找的流,找到成功后则返回streams[]中对应的序列号,否则返回-1
  videoindex =
    av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
  //通过解码器编号来遍历codec_list[]数组,来找到AVCodec
  pCodec =
    avcodec_find_decoder(pFormatCtx->streams[videoindex]->codecpar->codec_id);
  //构造AVCodecContext ,并将vcodec填入AVCodecContext中
  pCodecCtx = avcodec_alloc_context3(pCodec);
  //初始化AVCodecContext
  avcodec_parameters_to_context(pCodecCtx,
                                pFormatCtx->streams[videoindex]->codecpar);
  //打开解码器
  ret = avcodec_open2(pCodecCtx, NULL, NULL);
  if (ret != 0) {
    stop_ = true;
    be_ready_ = false;
    return ret;
  }
  pFrame = av_frame_alloc();
  pFrameRGB = av_frame_alloc();
  int size =
    avpicture_get_size(AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);
  out_buffer = (uint8_t *)av_malloc(size);

  avpicture_fill((AVPicture *)pFrameRGB, out_buffer, AV_PIX_FMT_RGB24,
                 pCodecCtx->width, pCodecCtx->height);
  packet = (AVPacket *)av_malloc(sizeof(AVPacket));
  img_convert_ctx = sws_getContext(
    pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width,
    pCodecCtx->height, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);

  int got_picture;
  cv::Mat dst_bgr;
  frame_list_ = std::make_shared<CircleBuffer<cv::Mat>>(max_);
  while (!stop_ && av_read_frame(pFormatCtx, packet) >= 0) {
    //如果是视频数据
    if (packet->stream_index == videoindex) {
      ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
      if (ret < 0) {
        printf("Decode Error.（解码错误）\n");
        stop_ = true;
        be_ready_ = false;
        return -1;
      }
      if (got_picture) {
        // YUV to RGB
        sws_scale(img_convert_ctx, (const uint8_t *const *)pFrame->data,
                  pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data,
                  pFrameRGB->linesize);
        cv::Mat src_rgb(pCodecCtx->height, pCodecCtx->width, CV_8UC3,
                        out_buffer);
        cvtColor(src_rgb, dst_bgr, cv::COLOR_RGB2BGR);
        while (frame_list_->size() >= max_) {
          be_ready_ = true;
          usleep(50 * kMillisecondUnit);
        }
        frame_list_->push(dst_bgr);
      }
    }
  }

  av_free(out_buffer);
  av_packet_free(&packet);
  av_frame_free(&pFrameRGB);
  av_frame_free(&pFrame);
  sws_freeContext(img_convert_ctx);
  img_convert_ctx = NULL;
  avcodec_close(pCodecCtx);
  avformat_close_input(&pFormatCtx);
  stop_ = true;
  be_ready_ = false;
  return 0;
}
bool ParseVideo::GetParseDate(cv::Mat &result) {
  if (frame_list_->size() == 0) {
    return false;
  }
  bool ret = frame_list_->pop(result);
  return ret;
};
}  // namespace pipeline