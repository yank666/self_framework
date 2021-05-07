//
// Created by yankai.yan on 2021/4/19.
//

#ifndef SELF_ARCHITECTURE_ERRORCODE_H
#define SELF_ARCHITECTURE_ERRORCODE_H
#include <string>

using STATUS = int;

/* Success */
constexpr int RET_OK = 0;                 /**< No error occurs. */
constexpr int RET_ERROR = -1;             /**< Common error code. */
constexpr int RET_NULL_PTR = -2;          /**< NULL pointer returned.*/
constexpr int RET_PARAM_INVALID = -3;     /**< Invalid parameter.*/
constexpr int RET_NO_CHANGE = -4;         /**< No change. */
constexpr int RET_SUCCESS_EXIT = -5;      /**< No error but exit. */
constexpr int RET_MEMORY_FAILED = -6;     /**< Fail to create memory. */
constexpr int RET_NOT_SUPPORT = -7;       /**< Fail to support. */
constexpr int RET_THREAD_POOL_ERROR = -8; /**< Error occur in thread pool. */
constexpr int RET_PARSE_ERROR = -9; /**< Error occur in thread pool. */

/* Executor error code, range: [-100,-200) */

/* Stage error code, range: [-300,-400) */
constexpr int RET_NOT_FIND_STAGE = -300;     /**< Failed to find operator. */
constexpr int RET_INVALID_STAGE_NAME = -301; /**< Invalid operator name. */
constexpr int RET_INVALID_OP_ATTR = -302;    /**< Invalid operator attr. */
constexpr int RET_OP_EXECUTE_STAGE = -303; /**< Failed to execution operator. */

/* Tensor error code, range: [-400,-500) */
constexpr int RET_FORMAT_ERR = -400; /**< Failed to checking tensor format. */

/* User input param error code, range: [-600, 700)*/
constexpr int RET_INPUT_FILE_INVALID =
  -600; /**< Invalid input param by user. */
constexpr int RET_INPUT_PARAM_INVALID =
  -601; /**< Invalid input param by user. */
#endif  // SELF_ARCHITECTURE_ERRORCODE_H
