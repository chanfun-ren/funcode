#pragma once

#include <iostream>

// 参考 redis 协议简单实现
// 0. 请求 Command:
// Redis协议使用"*"和"$"表示参数的数量和参数的长度。
// 对于SET命令，协议中应包含3个参数，分别为"SET"、键 和 值，
// 每个参数的格式为"$长度\r\n 参数值
// \r\n"。对于GET和RM命令，协议中应包含2个参数，分别为"GET"/"RM"和键，每个参数的格式同上。
// 1. 回复 Response:
// 改编Redis协议，使用"+"、"-"和"?"、"$"表示响应的类型，分别对应ResponseType::STATUS、ResponseType::ERROR和ResponseType::UNKNOWN。
// 响应消息的格式为"+/-/?/$message\r\n"。

namespace ckv {

enum CommandType { SET, GET, RM, INVALID };
struct Command {
  CommandType type;
  std::string key;
  std::string value;

  friend std::ostream& operator<<(std::ostream& os, const Command& cmd) {
    os << "Command: ";
    switch (cmd.type) {
      case CommandType::SET:
        os << "SET";
        break;
      case CommandType::GET:
        os << "GET";
        break;
      case CommandType::RM:
        os << "RM";
        break;
      default:
        os << "INVALID";
        break;
    }
    os << ", Key: " << cmd.key << ", Value: " << cmd.value;
    return os;
  }
};

enum ResponseType { STATUS, ERROR, UNKNOWN, DATA };
struct Response {
  ResponseType type;
  std::string message;

  friend std::ostream& operator<<(std::ostream& os, const Response& resp) {
    switch (resp.type) {
      case ResponseType::STATUS:
        os << "Status reply: " << resp.message;
        break;
      case ResponseType::ERROR:
        os << "Error reply: " << resp.message;
        break;
      case ResponseType::DATA:
        os << "Data reply: " << resp.message;
        break;
      default:
        os << "Unknown reply: " << resp.message;
        break;
    }
    return os;
  }
};

}  // namespace ckv
