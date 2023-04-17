#pragma once

namespace ckv {
enum class CommandType { SET, GET, RM, INVALID };

struct Command {
  CommandType type;
  std::string key;
  std::string value;
};
}  // namespace ckv
