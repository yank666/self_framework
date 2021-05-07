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

template <typename T>
class CircleBuffer {
 public:
  explicit CircleBuffer(size_t capacity = 10) {
    _capacityMask = capacity - 1;
    for(size_t i = 1; i <= sizeof(void*) * 4; i <<= 1)
      _capacityMask |= _capacityMask >> i;
    _capacity = _capacityMask + 1;

    _queue = (Node*)new char[sizeof(Node) * _capacity];
    for(size_t i = 0; i < _capacity; ++i) {
      _queue[i].tail.store(i, std::memory_order_relaxed);
      _queue[i].head.store(-1, std::memory_order_relaxed);
    }

    _tail.store(0, std::memory_order_relaxed);
    _head.store(0, std::memory_order_relaxed);
  }

  ~CircleBuffer() {
    for(size_t i = _head; i != _tail; ++i)
      (&_queue[i & _capacityMask].data)->~T();

    delete [] (char*)_queue;
  }

  size_t capacity() const {return _capacity;}

  size_t size() const {
    size_t head = _head.load(std::memory_order_acquire);
    return _tail.load(std::memory_order_relaxed) - head;
  }

  bool push(const T& data) {
    Node* node;
    size_t tail = _tail.load(std::memory_order_relaxed);
    for(;;) {
      node = &_queue[tail & _capacityMask];
      if(node->tail.load(std::memory_order_relaxed) != tail)
        return false;
      if((_tail.compare_exchange_weak(tail, tail + 1, std::memory_order_relaxed)))
        break;
    }
    new (&node->data)T(data);
    node->head.store(tail, std::memory_order_release);
    return true;
  }

  bool pop(T& result) {
    Node* node;
    size_t head = _head.load(std::memory_order_relaxed);
    for(;;) {
      node = &_queue[head & _capacityMask];
      if(node->head.load(std::memory_order_relaxed) != head)
        return false;
      if(_head.compare_exchange_weak(head, head + 1, std::memory_order_relaxed))
        break;
    }
    result = node->data;
    (&node->data)->~T();
    node->tail.store(head + _capacity, std::memory_order_release);
    return true;
  }

 private:
  struct Node {
    T data;
    std::atomic<size_t> tail;
    std::atomic<size_t> head;
  };

 private:
  size_t _capacityMask;
  Node* _queue;
  size_t _capacity;
  char cacheLinePad1[64];
  std::atomic<size_t> _tail;
  char cacheLinePad2[64];
  std::atomic<size_t> _head;
  char cacheLinePad3[64];
};

namespace pipeline {
class ParseVideo {
 public:
  ParseVideo() = default;
  ~ParseVideo() = default;
  int ParseVideoFromStream(const std::string stream_file,
                                  int32_t streamd_id);
  void SetParseBufferSize(const uint32_t max_num = 20);
  bool GetParseDate(cv::Mat &result);
  uint32_t GetVideoFrames();
  bool GetReady();
 protected:
   double stream_fps_;
   uint32_t stream_frames_;
   uint32_t stream_width_;
   uint32_t stream_height_;
   bool stop_;
   uint32_t max_;
   CircleBuffer<cv::Mat> frame_list_;
   bool be_ready_{false};
  private:
   cv::Mat ret_;
   std::mutex task_mutex_;
};

}
#endif  // SELF_ARCHITECTURE_PARSE_VIDEO_H
