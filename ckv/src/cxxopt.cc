#include <iostream>
#include <string>
#include <unordered_map>

#include "cxxopts.h"

int main(int argc, char** argv) {
  cxxopts::Options options("kvs", "A simple key-value store");

  options.add_options()("h,help", "Print help message")(
      "V,version", "Print version")("c,command", "Command to execute",
                                    cxxopts::value<std::string>())(
      "k,key", "Key to operate", cxxopts::value<std::string>())(
      "v,value", "Value to set", cxxopts::value<std::string>());

  options.parse_positional({"command", "key", "value"});
  options.positional_help("[command] [key] [value]");

  auto result = options.parse(argc, argv);

  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    return 0;
  }

  if (result.count("version")) {
    std::cout << "kvs version 1.0.0" << std::endl;
    return 0;
  }

  if (!result.count("command") || !result.count("key")) {
    std::cerr << "Missing required argument: command or key" << std::endl;
    return 1;
  }

  std::string command = result["command"].as<std::string>();
  std::string key = result["key"].as<std::string>();

  if (command == "get") {
    std::cout << "Get value of key `" << key << "`" << std::endl;
  } else if (command == "set") {
    if (!result.count("value")) {
      std::cerr << "Missing required argument: value" << std::endl;
      return 1;
    }
    std::string value = result["value"].as<std::string>();
    std::cout << "Set key `" << key << "` to value `" << value << "`"
              << std::endl;
  } else if (command == "rm") {
    std::cout << "Remove key `" << key << "`" << std::endl;
  } else {
    std::cerr << "Invalid command: " << command << std::endl;
    return 1;
  }

  return 0;
}