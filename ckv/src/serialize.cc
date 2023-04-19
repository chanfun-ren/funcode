#include "serialize.h"

namespace ckv {

//------------------- kv_store log usage -------------------------/
std::string serialize(Command cmd) {
  std::string log_entry;
  switch (cmd.type) {
    case CommandType::SET: {
      log_entry = cmd.key + " " + cmd.value + "\n";
      break;
    }
    case CommandType::RM: {
      std::string empty_value = "";
      log_entry = cmd.key + " " + empty_value + "\n";
      break;
    }
  }
  return log_entry;
}

Command deserialize(std::string line) {
  auto pos = line.find(' ');
  if (pos == std::string::npos) {
    return Command{CommandType::INVALID, "", ""};
  }
  if (pos == line.size() - 1) {
    // `rm` command log entry
    std::string key = line.substr(0, pos);
    return Command{CommandType::RM, key, ""};
  } else {
    // `set` command log entry
    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);
    return Command{CommandType::SET, key, value};
  }
}

//----------------------------------------------------------------/

//------------------- Command network usag------------------------/
// INVALID command should not be serialized/deserialized
int serialize(const Command& cmd, std::string& out) {
  out.clear();
  switch (cmd.type) {
    case CommandType::SET: {
      out += "*" + std::to_string(3) + "\r\n";
      out += "$" + std::to_string(3) + "\r\n" + "SET" + "\r\n";
      out += "$" + std::to_string(cmd.key.size()) + "\r\n" + cmd.key + "\r\n";
      out +=
          "$" + std::to_string(cmd.value.size()) + "\r\n" + cmd.value + "\r\n";
      break;
    }
    case CommandType::GET: {
      out += "*" + std::to_string(2) + "\r\n";
      out += "$" + std::to_string(3) + "\r\n" + "GET" + "\r\n";
      out += "$" + std::to_string(cmd.key.size()) + "\r\n" + cmd.key + "\r\n";
      break;
    }
    case CommandType::RM: {
      out += "*" + std::to_string(2) + "\r\n";
      out += "$" + std::to_string(2) + "\r\n" + "RM" + "\r\n";
      out += "$" + std::to_string(cmd.key.size()) + "\r\n" + cmd.key + "\r\n";
      break;
    }
    default: {
      return -1;
    }
  }
  return 0;
}

int deserialize(const std::string& in, Command& cmd) {
  int pos = 0;
  int arg_count = 0;
  if (in[pos] != '*') {
    return -1;
  }
  ++pos;
  int r_pos = in.find('\r', pos);
  int n_pos = r_pos + 1;
  if (in[n_pos] != '\n') {
    return -1;
  }
  arg_count = std::stoi(in.substr(pos, r_pos - pos));
  if (arg_count != 3 && arg_count != 2) {
    // set k v, rm k, get k
    // cmd.type = CommandType::INVALID;
    return -1;
  }

  pos = n_pos + 1;
  for (int i = 0; i < arg_count; ++i) {
    // 解析字段长度
    if (in[pos] != '$') {
      return -1;
    }
    int r_pos = in.find('\r', pos);
    int n_pos = r_pos + 1;
    if (in[n_pos] != '\n') {
      return -1;
    }
    ++pos;
    int field_len = std::stoi(in.substr(pos, r_pos - pos));

    // 解析字段
    pos = n_pos + 1;
    std::string field = in.substr(pos, field_len);
    if (i == 0) {
      if (field == "SET") {
        cmd.type = CommandType::SET;
      } else if (field == "GET") {
        cmd.type = CommandType::GET;
      } else if (field == "RM") {
        cmd.type = CommandType::RM;
      } else {
        // cmd.type = CommandType::INVALID;
        return -1;
      }
    } else if (arg_count == 2 && i == 1) {
      cmd.key = field;
    } else if (arg_count == 3) {
      if (i == 1) {
        cmd.key = field;
      } else if (i == 2) {
        cmd.value = field;
      }
    }
    r_pos = in.find('\r', pos);
    n_pos = r_pos + 1;
    if (in[n_pos] != '\n') {
      return -1;
    }
    pos = n_pos + 1;
  }

  return 0;
}

//----------------------------------------------------------------/

//------------------- Response network usag------------------------/
int serialize(const Response& resp, std::string& str) {
  switch (resp.type) {
    case ResponseType::STATUS:
      str = "+" + resp.message + "\r\n";
      break;
    case ResponseType::ERROR:
      str = "-" + resp.message + "\r\n";
      break;
    case ResponseType::DATA:
      str = '$' + resp.message + "\r\n";
      break;
    case ResponseType::UNKNOWN:
      str = "?" + resp.message + "\r\n";
      break;
    default:
      return -1;
  }
  return 0;
}

int deserialize(const std::string& str, Response& resp) {
  if (str.empty()) {
    return -1;
  }

  switch (str[0]) {
    case '+':
      resp.type = ResponseType::STATUS;
      break;
    case '-':
      resp.type = ResponseType::ERROR;
      break;
    case '?':
      resp.type = ResponseType::UNKNOWN;
      break;
    case '$':
      resp.type = ResponseType::DATA;
      break;
    default:
      return -1;
  }

  size_t pos = str.find("\r\n", 1);
  if (pos == std::string::npos) {
    return -1;
  }

  resp.message = str.substr(1, pos - 1);
  return 0;
}
//----------------------------------------------------------------/

}  // namespace ckv
