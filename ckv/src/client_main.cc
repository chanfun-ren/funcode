#include <iostream>
#include <string>

#include "cmdline.h"
#include "kv_client.h"
#include "protocol.h"
#include "utils/addr_util.h"

using namespace ckv;

struct Args {
  Command cmd;
  std::string addr;
};

const std::string DEFAULT_ADDR = "127.0.0.1:8888";

CommandType parse_command_type(const std::string& command) {
  if (command == "set") {
    return CommandType::SET;
  } else if (command == "get") {
    return CommandType::GET;
  } else if (command == "rm") {
    return CommandType::RM;
  } else {
    return CommandType::INVALID;
  }
}

std::vector<std::string> split_input(const std::string& input) {
  std::stringstream ss(input);
  std::vector<std::string> result;
  std::string token;
  while (ss >> token) {
    result.push_back(token);
  }
  return result;
}

Args parse_args(const std::vector<std::string>& args) {
  Command cmd = {CommandType::INVALID, "", ""};
  const Args invalid = {cmd, ""};

  Args res = {cmd, ""};
  if (args.empty()) {
    return invalid;
  }

  CommandType type = parse_command_type(args[0]);
  if (type == CommandType::INVALID) {
    return invalid;
  }
  res.cmd.type = type;
  res.addr = DEFAULT_ADDR;

  if (type == CommandType::SET && (args.size() == 3 || args.size() == 4)) {
    // SET command
    res.cmd.key = args[1];
    res.cmd.value = args[2];
    if (args.size() == 4) {
      res.addr = args[3];
    }
  } else if ((type == CommandType::GET || type == CommandType::RM) &&
             (args.size() == 2 || args.size() == 3)) {
    // GET/RM command
    res.cmd.key = args[1];
    if (args.size() == 3) {
      res.addr = args[2];
    }
  } else {
    // INVALID command
    return invalid;
  }

  return is_valid_addr(res.addr) ? res : invalid;
}

// Print the usage of the program
void print_usage(const char* prog_name) {
  std::cerr << "Usage: " << prog_name
            << " <command> <key> [value] [--addr IP-PORT]\n"
            << "Commands:\n"
            << "  set    Set the value of a string key to a string\n"
            << "  get    Get the string value of a given string key\n"
            << "  rm     Remove a given key\n"
            << "Options:\n"
            << "  --addr IP-PORT    Address and port of the server (default: "
               "127.0.0.1:8888)\n";
}

void run(KvClient& client) {
  while (true) {
    // Read user input
    std::string input;
    std::cout << "> ";
    std::getline(std::cin, input);

    // Parse args
    std::vector<std::string> params = split_input(input);
    Args args = parse_args(params);

    if (args.cmd.type == CommandType::INVALID) {
      std::cerr << "Invalid command" << std::endl;
      print_usage("kv-client");
      continue;
    }

    Command command = args.cmd;
    Response response;
    // Execute command
    switch (command.type) {
      // 用户可利用 kv-client 中途更换服务器地址
      // TODO: 0.KvClient 建立新连接 1.logging
      case CommandType::SET: {
        std::string key = command.key;
        std::string value = command.value;
        // std::cout << "Set key `" << key << "` to value `" << value << "`"
        //           << std::endl;
        Status status = client.ExecCmd(command, response);
        if (status.isOk()) {
          // std::cout << "Set operation send succeeded" << std::endl;
        } else {
          std::cerr << "Set operation send failed: " << status.message()
                    << std::endl;
        }
        // TODO: parse Response type
        std::cout << response.message << std::endl;
        break;
      }
      case CommandType::GET: {
        std::string key = command.key;
        // std::cout << "Get value of key `" << key << "`" << std::endl;
        Status status = client.ExecCmd(command, response);
        if (status.isOk()) {
          // std::cout << "Get operation send succeeded" << std::endl;
        } else {
          std::cerr << "Get operation send failed: " << status.message()
                    << std::endl;
        }
        // TODO: parse Response type
        std::cout << response.message << std::endl;
        break;
      }
      case CommandType::RM: {
        std::string key = command.key;
        // std::cout << "Remove key `" << key << "`" << std::endl;
        Status status = client.ExecCmd(command, response);
        if (status.isOk()) {
          // std::cout << "Remove operation send succeeded" << std::endl;
        } else {
          std::cerr << "Remove operation send failed: " << status.message()
                    << std::endl;
        }
        std::cout << response.message << std::endl;
        break;
      }
    }
  }
}

int main(int argc, char** argv) {
  std::string addr = (argc == 1) ? DEFAULT_ADDR : argv[1];
  if (!is_valid_addr(addr)) {
    std::cout << "invalid ip_port";
    return -1;
  }
  KvClient client(addr);
  run(client);

  return 0;
}