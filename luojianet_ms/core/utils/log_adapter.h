/**
 * Copyright 2021, 2022 LuoJiaNET Research and Development Group, Wuhan University
 * Copyright 2021, 2022 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LUOJIANET_MS_CORE_UTILS_LOG_ADAPTER_H_
#define LUOJIANET_MS_CORE_UTILS_LOG_ADAPTER_H_

#include <stdarg.h>
#include <stdint.h>
#include <string>
#include <sstream>
#include <memory>
#include <map>
#include <thread>
#include <functional>
#include "utils/visible.h"
#include "utils/overload.h"
#include "./securec.h"
#ifdef USE_GLOG
#define GLOG_NO_ABBREVIATED_SEVERITIES
#define google luojianet_ms_private
#include "glog/logging.h"
#undef google
#else
#include "toolchain/slog.h"
#endif
// NOTICE: when relative path of 'log_adapter.h' changed, macro 'LOG_HDR_FILE_REL_PATH' must be changed
#define LOG_HDR_FILE_REL_PATH "luojianet_ms/core/utils/log_adapter.h"

// Get start index of file relative path in __FILE__
static constexpr size_t GetRelPathPos() noexcept {
  return sizeof(__FILE__) > sizeof(LOG_HDR_FILE_REL_PATH) ? sizeof(__FILE__) - sizeof(LOG_HDR_FILE_REL_PATH) : 0;
}

namespace luojianet_ms {
/// \brief The handler map for ACL.
MS_CORE_API extern std::map<void **, std::thread *> acl_handle_map;
#define FILE_NAME                                                                             \
  (sizeof(__FILE__) > GetRelPathPos() ? static_cast<const char *>(__FILE__) + GetRelPathPos() \
                                      : static_cast<const char *>(__FILE__))
enum ExceptionType {
  NoExceptionType = 0,
  UnknownError,
  ArgumentError,
  NotSupportError,
  NotExistsError,
  AlreadyExistsError,
  UnavailableError,
  DeviceProcessError,
  AbortedError,
  TimeOutError,
  ResourceUnavailable,
  NoPermissionError,
  IndexError,
  ValueError,
  TypeError,
  KeyError,
  AttributeError,
  NameError
};

struct LocationInfo {
  LocationInfo(const char *file, int line, const char *func) : file_(file), line_(line), func_(func) {}
  ~LocationInfo() = default;

  const char *file_;
  int line_;
  const char *func_;
};

template <class T, typename std::enable_if<std::is_enum<T>::value, int>::type = 0>
constexpr std::ostream &operator<<(std::ostream &stream, const T &value) {
  return stream << static_cast<typename std::underlying_type<T>::type>(value);
}

class LogStream {
 public:
  LogStream() { sstream_ = std::make_shared<std::stringstream>(); }
  ~LogStream() = default;

  template <typename T>
  LogStream &operator<<(const T &val) noexcept {
    (*sstream_) << val;
    return *this;
  }

  LogStream &operator<<(std::ostream &func(std::ostream &os)) noexcept {
    (*sstream_) << func;
    return *this;
  }

  friend class LogWriter;

 private:
  std::shared_ptr<std::stringstream> sstream_;
};

enum MsLogLevel : int { DEBUG = 0, INFO, WARNING, ERROR, EXCEPTION };

enum SubModuleId : int {
  SM_UNKNOWN = 0,        // unknown submodule
  SM_CORE,               // core
  SM_ANALYZER,           // static analyzer
  SM_COMMON,             // common
  SM_DEBUG,              // debug
  SM_OFFLINE_DEBUG,      // offline debug
  SM_DEVICE,             // device
  SM_GE_ADPT,            // ge adapter
  SM_IR,                 // IR
  SM_KERNEL,             // kernel
  SM_MD,                 // MindData
  SM_ME,                 // MindExpression
  SM_EXPRESS,            // EXPRESS_IR
  SM_OPTIMIZER,          // optimzer
  SM_PARALLEL,           // parallel
  SM_PARSER,             // parser
  SM_PIPELINE,           // ME pipeline
  SM_PRE_ACT,            // pre-activate
  SM_PYNATIVE,           // PyNative
  SM_SESSION,            // session
  SM_UTILS,              // utils
  SM_VM,                 // VM
  SM_PROFILER,           // profiler
  SM_PS,                 // Parameter Server
  SM_FL,                 // Federated Learning
  SM_DISTRIBUTED,        // Distributed
  SM_LITE,               // LITE
  SM_ARMOUR,             // ARMOUR
  SM_HCCL_ADPT,          // Hccl Adapter
  SM_RUNTIME_FRAMEWORK,  // Runtime framework
  SM_GE,                 // GraphEngine
  SM_API,                // MindAPI
  NUM_SUBMODUES          // number of submodules
};

#ifndef SUBMODULE_ID
#define SUBMODULE_ID luojianet_ms::SubModuleId::SM_ME
#endif

/// \brief Get sub-module name by the module id.
///
/// \param[in] module_id The module id.
///
/// \return The sub-module name.
MS_EXPORT const std::string GetSubModuleName(SubModuleId module_id);

/// \brief Get current time as a string.
///
/// \return The string presents current time.
MS_EXPORT std::string GetTimeString();

/// \brief The log levels of luojianet_ms sub-module.
MS_EXPORT extern int g_ms_submodule_log_levels[];

#if defined(_WIN32) || defined(_WIN64)
/// \brief The max log level of current thread.
MS_EXPORT extern enum MsLogLevel this_thread_max_log_level;
#define MS_LOG_TRY_CATCH_SCOPE
#else
/// \brief The max log level of current thread.
MS_EXPORT extern thread_local enum MsLogLevel this_thread_max_log_level;
class TryCatchGuard {
 public:
  TryCatchGuard() {
    origin_log_level_ = this_thread_max_log_level;
    this_thread_max_log_level = MsLogLevel::WARNING;
  }

  ~TryCatchGuard() { this_thread_max_log_level = origin_log_level_; }

 private:
  enum MsLogLevel origin_log_level_;
};
#define MS_LOG_TRY_CATCH_SCOPE luojianet_ms::TryCatchGuard luojianet_log_try_catch_guard
#endif

/// \brief LogWriter defines interface to write log.
class LogWriter {
 public:
  using ExceptionHandler = std::function<void(ExceptionType, const std::string &msg)>;
  using TraceProvider = std::function<void(std::ostringstream &oss)>;

  LogWriter(const LocationInfo &location, MsLogLevel log_level, SubModuleId submodule,
            ExceptionType excp_type = NoExceptionType)
      : location_(location), log_level_(log_level), submodule_(submodule), exception_type_(excp_type) {}
  ~LogWriter() = default;

  /// \brief Output log message from the input log stream.
  ///
  /// \param[in] stream The input log stream.
  MS_CORE_API void operator<(const LogStream &stream) const noexcept;

  /// \brief Output log message from the input log stream and then throw exception.
  ///
  /// \param[in] stream The input log stream.
  MS_CORE_API void operator^(const LogStream &stream) const __attribute__((noreturn));

  static void set_exception_handler(ExceptionHandler exception_handler) { exception_handler_ = exception_handler; }
  static void set_trace_provider(TraceProvider trace_provider) { trace_provider_ = trace_provider; }
  static TraceProvider trace_provider() { return trace_provider_; }

 private:
  void OutputLog(const std::ostringstream &msg) const;

  LocationInfo location_;
  MsLogLevel log_level_;
  SubModuleId submodule_;
  ExceptionType exception_type_;

  inline static ExceptionHandler exception_handler_ = nullptr;
  inline static TraceProvider trace_provider_ = nullptr;
};

#define MSLOG_IF(level, condition, excp_type)                                                                       \
  static_cast<void>(0), !(condition)                                                                                \
                          ? void(0)                                                                                 \
                          : luojianet_ms::LogWriter(luojianet_ms::LocationInfo(FILE_NAME, __LINE__, __FUNCTION__), level, \
                                                 SUBMODULE_ID, excp_type) < luojianet_ms::LogStream()
#define MSLOG_THROW(excp_type)                                                                                         \
  luojianet_ms::LogWriter(luojianet_ms::LocationInfo(FILE_NAME, __LINE__, __FUNCTION__), luojianet_ms::EXCEPTION, SUBMODULE_ID, \
                       excp_type) ^                                                                                    \
    luojianet_ms::LogStream()

#define IS_OUTPUT_ON(level) \
  ((level) >= luojianet_ms::g_ms_submodule_log_levels[SUBMODULE_ID] && (level) <= luojianet_ms::this_thread_max_log_level)

#define MS_LOG(level) MS_LOG_##level

#define MS_LOG_DEBUG MSLOG_IF(luojianet_ms::DEBUG, IS_OUTPUT_ON(luojianet_ms::DEBUG), luojianet_ms::NoExceptionType)
#define MS_LOG_INFO MSLOG_IF(luojianet_ms::INFO, IS_OUTPUT_ON(luojianet_ms::INFO), luojianet_ms::NoExceptionType)
#define MS_LOG_WARNING MSLOG_IF(luojianet_ms::WARNING, IS_OUTPUT_ON(luojianet_ms::WARNING), luojianet_ms::NoExceptionType)
#define MS_LOG_ERROR MSLOG_IF(luojianet_ms::ERROR, IS_OUTPUT_ON(luojianet_ms::ERROR), luojianet_ms::NoExceptionType)

#define MS_LOG_EXCEPTION MSLOG_THROW(luojianet_ms::NoExceptionType)
#define MS_EXCEPTION(type) MSLOG_THROW(type)
}  // namespace luojianet_ms

#define MS_EXCEPTION_IF_NULL(ptr)                                    \
  do {                                                               \
    if ((ptr) == nullptr) {                                          \
      MS_LOG(EXCEPTION) << ": The pointer[" << #ptr << "] is null."; \
    }                                                                \
  } while (0)

#define MS_EXCEPTION_IF_CHECK_FAIL(condition, error_info)              \
  do {                                                                 \
    if (!(condition)) {                                                \
      MS_LOG(EXCEPTION) << ": Failure info [" << (error_info) << "]."; \
    }                                                                  \
  } while (0)

#define MS_EXCEPTION_IF_ZERO(name, value)                   \
  do {                                                      \
    if (value == 0) {                                       \
      MS_LOG(EXCEPTION) << ": The " << name << " is zero."; \
    }                                                       \
  } while (0)

#define MS_ERROR_IF_NULL(ptr)                                    \
  do {                                                           \
    if ((ptr) == nullptr) {                                      \
      MS_LOG(ERROR) << ": The pointer[" << #ptr << "] is null."; \
      return false;                                              \
    }                                                            \
  } while (0)

#define MS_ERROR_IF_NULL_W_RET_VAL(ptr, val)                     \
  do {                                                           \
    if ((ptr) == nullptr) {                                      \
      MS_LOG(ERROR) << ": The pointer[" << #ptr << "] is null."; \
      return val;                                                \
    }                                                            \
  } while (0)

#define MS_ERROR_IF_NULL_WO_RET_VAL(ptr)                         \
  do {                                                           \
    if ((ptr) == nullptr) {                                      \
      MS_LOG(ERROR) << ": The pointer[" << #ptr << "] is null."; \
      return;                                                    \
    }                                                            \
  } while (0)

#define RETURN_IF_FALSE(condition) \
  do {                             \
    if (!(condition)) {            \
      return false;                \
    }                              \
  } while (false)

#define RETURN_IF_FALSE_WITH_LOG(condition, message) \
  do {                                               \
    if (!(condition)) {                              \
      MS_LOG(ERROR) << message;                      \
      return false;                                  \
    }                                                \
  } while (false)

#ifdef DEBUG
#include <cassert>
#define MS_ASSERT(f) assert(f)
#else
#define MS_ASSERT(f) ((void)0)
#endif

#endif  // LUOJIANET_MS_CORE_UTILS_LOG_ADAPTER_H_
