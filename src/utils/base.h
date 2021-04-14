//
// Created by yankai.yan on 2021/4/14.
//

#ifndef SELF_ARCHITECTURE_SRC_UTILS_BASE_H
#define SELF_ARCHITECTURE_SRC_UTILS_BASE_H

#include "glog/logging.h"
#include <string>
#include <memory>
// check the null point
#define REPORT_ERROR_IF_NULL(ptr)                                \
  do {                                                           \
    if ((ptr) == nullptr) {                                      \
      LOG(ERROR) << ": The pointer[" << #ptr << "] is null.";    \
      return false;                                              \
    }                                                            \
  } while (0)

#define REPORT_EXCEPTION_IF_NULL(ptr)                                \
  do {                                                               \
    if ((ptr) == nullptr) {                                          \
      LOG(FATAL) << ": The pointer[" << #ptr << "] is null.";    \
    }                                                                \
  } while (0)

//#define REPOER_LOG_IF(level, condition, excp_type)                                                                       \
//   !(condition) ? void(0)                                                                                 \
//                          : mindspore::LogWriter(mindspore::LocationInfo(FILE_NAME, __LINE__, __FUNCTION__), level, \
//                                                 SUBMODULE_ID, excp_type) < mindspore::LogStream()

#endif // SELF_ARCHITECTURE_SRC_UTILS_BASE_H

