#include <algorithm>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "command.h"
#include "kv_store.h"
#include "status.h"

using namespace ckv;

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

Command parse_command(const std::vector<std::string>& args) {
  if (args.empty()) {
    return {CommandType::INVALID, "", ""};
  }

  CommandType type = parse_command_type(args[0]);
  if (type == CommandType::INVALID) {
    return {CommandType::INVALID, "", ""};
  }

  std::string key;
  std::string value;
  if (type == CommandType::SET && args.size() == 3) {
    key = args[1];
    value = args[2];
  } else if ((type == CommandType::GET || type == CommandType::RM) &&
             args.size() == 2) {
    key = args[1];
  } else {
    return {CommandType::INVALID, "", ""};
  }

  return {type, key, value};
}

// std::unique_ptr<KvStore> init_db(std::string_view db_path) {
//   std::unique_ptr<KvStore> db = std::make_unique<KvStore>(db_path);
//   Status status = db->Open();
//   if (status.isOk()) {
//     return db;
//   }
//   std::cerr << status.message() << std::endl;
//   return nullptr;
// }

int main(int argc, char** argv) {
  // init db
  std::unique_ptr<KvStore> db_ptr = std::make_unique<KvStore>("./db");

  while (true) {
    // Read user input
    std::string input;
    std::cout << "> ";
    std::getline(std::cin, input);

    // Parse command
    std::vector<std::string> args = split_input(input);
    Command command = parse_command(args);

    // Execute command
    switch (command.type) {
      case CommandType::SET: {
        std::string key = command.key;
        std::string value = command.value;
        std::cout << "Set key `" << key << "` to value `" << value << "`"
                  << std::endl;
        Status status = db_ptr->Set(key, value);
        if (status.isOk()) {
          std::cout << "Set operation succeeded" << std::endl;
        } else {
          std::cerr << "Set operation failed: " << status.message()
                    << std::endl;
        }
        break;
      }
      case CommandType::GET: {
        std::string key = command.key;
        std::cout << "Get value of key `" << key << "`" << std::endl;
        std::string value;
        Status status = db_ptr->Get(key, value);
        if (status.isOk()) {
          std::cout << "Value of key `" << key << "` is `" << value << "`"
                    << std::endl;
        } else if (status.code() == kNotFound) {
          std::cerr << "Key `" << key << "` not found" << std::endl;
        } else {
          std::cerr << "Get operation failed: " << status.message()
                    << std::endl;
        }
        break;
      }
      case CommandType::RM: {
        std::string key = command.key;
        std::cout << "Remove key `" << key << "`" << std::endl;
        Status status = db_ptr->Remove(key);
        if (status.isOk()) {
          std::cout << "Remove operation succeeded" << std::endl;
        } else if (status.code() == kNotFound) {
          std::cerr << "Key `" << key << "` not found" << std::endl;
        } else {
          std::cerr << "Remove operation failed: " << status.message()
                    << std::endl;
        }
        break;
      }
      case CommandType::INVALID: {
        std::cerr << "Invalid command" << std::endl;
        break;
      }
    }
  }

  return 0;
}