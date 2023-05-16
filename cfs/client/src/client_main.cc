#include <memory>
#include <sstream>
#include <vector>

#include "cilent.h"

using cfs::Client;

void RunClient() {
  std::string addr = "0.0.0.0:5051";
  std::shared_ptr<grpc::Channel> channel =
      grpc::CreateChannel(addr, grpc::InsecureChannelCredentials());
  Client cli(channel);
  // cli.Stat("/");
  // cli.Mkdir("/a");
  // cli.List("/");

  while (true) {
    std::cout << "cfs> ";
    std::string line;
    std::getline(std::cin, line);
    std::istringstream line_stream(line);
    std::string cmd;
    line_stream >> cmd;
    if (cmd == "stat") {
      std::vector<std::string> v;
      std::string temp;
      while (line_stream >> temp) {
        v.push_back(temp);
      }
      if (v.size() == 1) {
        cli.Stat(v[0]);
        continue;
      }
      std::cout << "Stat Error\n";
    } else if (cmd == "list") {
      std::vector<std::string> v;
      std::string temp;
      while (line_stream >> temp) {
        v.push_back(temp);
      }
      if (v.size() == 1) {
        cli.List(v[0]);
        continue;
      }
      std::cout << "List Error\n";
    } else if (cmd == "mkdir") {
      std::vector<std::string> v;
      std::string temp;
      while (line_stream >> temp) {
        v.push_back(temp);
      }
      //	  std::cout<<v.size()<<"\n";
      if (v.size() == 1) {
        cli.Mkdir(v[0]);
        continue;
      }
      std::cout << "Mkdir Error\n";
    } else if (cmd == "rename") {
      std::vector<std::string> v;
      std::string temp;
      while (line_stream >> temp) {
        v.push_back(temp);
      }
      if (v.size() == 2) {
        cli.Rename(v[0], v[1]);
        continue;
      }
      std::cout << "Rename Error\n";
    } else if (cmd == "get") {
      std::vector<std::string> v;
      std::string temp;
      while (line_stream >> temp) {
        v.push_back(temp);
      }
      if (v.size() == 2) {
        cli.Get(v[0], v[1]);
        continue;
      }
      std::cout << "Get Error\n";
    } else if (cmd == "put") {
      std::vector<std::string> v;
      std::string temp;
      while (line_stream >> temp) {
        v.push_back(temp);
      }
      if (v.size() == 2) {
        cli.Put(v[0], v[1]);
        continue;
      }
      std::cout << "Put Error\n";
    } else if (cmd == "help" || cmd == "h") {
      std::cout << "Options:\n"
                << "\tget\t<remotefilepath>\t<filepath>\n"
                << "\tput\t<remotefilepath>\t<filepath>\n"
                << "\trename\t<oldname>\t\t<newname>\n"
                << "\tstat\t<remotefialpath>\n"
                << "\tdelete\t<remotefilepath>\n"
                << "\tmkdir\t<remotefilepath>\n"
                << "\tlist\t<remotedirpath>\n";
      continue;
    } else if (cmd == "clear" || cmd == "clr") {
      std::cout << "\033c\n";
      continue;
    } else if (cmd == "quit" || cmd == "q") {
      std::cout << "Quit Successfully\n";
      break;
    }
    std::cout << "Invalid Input\n";
    std::cout << "You can input \"help\" or \"h\" show the help message\n";
    continue;
  }
}

void SetLogger() {
  std::vector<spdlog::sink_ptr> sinks;
  sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_st>());
  sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_st>(
      "logs/clientlog", 23, 59));
  auto combined_logger = std::make_shared<spdlog::logger>(
      "combined_logger", begin(sinks), end(sinks));

  combined_logger->set_level(spdlog::level::trace);  // TODO: load_env_levels()
  combined_logger->flush_on(spdlog::level::trace);

  // register it if you need to access it globally
  spdlog::register_logger(combined_logger);
  spdlog::set_default_logger(combined_logger);
  spdlog::set_pattern("%+");
}

int main() {
  SetLogger();
  RunClient();
  return 0;
}