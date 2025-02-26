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

#ifndef LUOJIANET_MS_CCSRC_DISTRIBUTED_RPC_TCP_CONSTANTS_H_
#define LUOJIANET_MS_CCSRC_DISTRIBUTED_RPC_TCP_CONSTANTS_H_

#include <string>
#include <csignal>
#include <queue>

#include "actor/log.h"

namespace luojianet_ms {
namespace distributed {
namespace rpc {
using DeleteCallBack = void (*)(const std::string &from, const std::string &to);
using ConnectionCallBack = void (*)(void *conn);

constexpr int SEND_MSG_IO_VEC_LEN = 5;
constexpr int RECV_MSG_IO_VEC_LEN = 4;

constexpr unsigned int BUSMAGIC_LEN = 4;
constexpr int SENDMSG_QUEUELEN = 1024;
constexpr int SENDMSG_DROPED = -1;

constexpr size_t MAX_KMSG_FROM_LEN = 1024;
constexpr size_t MAX_KMSG_TO_LEN = 1024;
constexpr size_t MAX_KMSG_NAME_LEN = 1024;
constexpr size_t MAX_KMSG_BODY_LEN = 104857600;

enum ParseType { kTcpMsg = 1, kHttpReq, kHttpRsp, kUnknown };
enum State { kMagicId = 1, kMsgHeader, kName, kDestination, kSource, kBody };
enum ConnectionState { kInit = 1, kConnecting, kConnected, kDisconnecting, kClose };
enum ConnectionType { kTcp = 1, kSSL };
enum ConnectionPriority { kPriorityLow = 1, kPriorityHigh };

static const int g_httpKmsgEnable = -1;

using IntTypeMetrics = std::queue<int>;
using StringTypeMetrics = std::queue<std::string>;

// Server socket listen backlog.
static const int SOCKET_LISTEN_BACKLOG = 2048;

static const int SOCKET_KEEPALIVE = 1;

// Send first probe after `interval' seconds.
static const int SOCKET_KEEPIDLE = 600;

// Send next probes after the specified interval.
static const int SOCKET_KEEPINTERVAL = 5;

// Consider the socket in error state after we send three ACK
// probes without getting a reply.
static const int SOCKET_KEEPCOUNT = 3;

static const char RPC_MAGICID[] = "BUS0";
static const char URL_PROTOCOL_IP_SEPARATOR[] = "://";
static const char URL_IP_PORT_SEPARATOR[] = ":";
static const char TCP_RECV_EVLOOP_THREADNAME[] = "RECV_EVENT_LOOP";
static const char TCP_SEND_EVLOOP_THREADNAME[] = "SEND_EVENT_LOOP";

constexpr int RPC_ERROR = -1;
constexpr int RPC_OK = 0;

constexpr int IP_LEN_MAX = 128;

// Kill the process for safe exiting.
inline void KillProcess(const std::string &ret) { raise(SIGKILL); }

#define RPC_ASSERT(expression)                                                                       \
  do {                                                                                               \
    if (!(expression)) {                                                                             \
      std::stringstream ss;                                                                          \
      ss << "Assertion failed: " << #expression << ", file: " << __FILE__ << ", line: " << __LINE__; \
      KillProcess(ss.str());                                                                         \
    }                                                                                                \
  } while (0)

#define RPC_EXIT(ret)                                                           \
  do {                                                                          \
    std::stringstream ss;                                                       \
    ss << (ret) << "  ( file: " << __FILE__ << ", line: " << __LINE__ << " )."; \
    KillProcess(ss.str());                                                      \
  } while (0)

#define RPC_OOM_EXIT(ptr)        \
  {                              \
    if (ptr == nullptr) {        \
      RPC_EXIT("Exit for OOM."); \
    }                            \
  }
}  // namespace rpc
}  // namespace distributed
}  // namespace luojianet_ms
#endif
