#include <sstream>

#include "client.h"
#include "metadata.pb.h"

// INITIALIZE_EASYLOGGINGPP

// void InitLogConf(const char *log_path) {
//   el::Configurations conf(log_path);
//   el::Loggers::reconfigureAllLoggers(conf);
// }

void RunClient() {
  std::vector<std::string> master_addrs = {"0.0.0.0:50051", "0.0.0.0:50052",
                                           "0.0.0.0:50053"};
  std::vector<std::shared_ptr<grpc::Channel>> master_channels;
  for (auto addr : master_addrs) {
    master_channels.push_back(
        grpc::CreateChannel(addr, grpc::InsecureChannelCredentials()));
  }

  int client_id = 22222;
  Client cli(master_channels, client_id);

  while (true) {
    std::cout << "cfs> ";
    std::string line;
    std::getline(std::cin, line);
    std::istringstream line_stream(line);
    std::string cmd;
    line_stream >> cmd;
    if (cmd == "get") {
      std::string remotepath, filepath;
      std::vector<std::string> v;
      std::string temp;
      while (line_stream >> temp) {
        v.push_back(temp);
      }
      //		std::cout<<v.size()<<"\n";
      if (v.size() == 2) {
        std::cout << "Get Runnning\n";
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
      //	  std::cout<<v.size()<<"\n";
      if (v.size() == 2) {
        std::cout << "Put Running\n";
        cli.Put(v[0], v[1]);
        continue;
      }
      std::cout << "Put Error\n";
    } else if (cmd == "rename") {
      std::vector<std::string> v;
      std::string temp;
      while (line_stream >> temp) {
        v.push_back(temp);
      }
      //	  std::cout<<v.size()<<"\n";
      if (v.size() == 2) {
        std::cout << "Rename Running\n";
        cli.Rename(v[0], v[1]);
        continue;
      }
      std::cout << "Rename Error\n";
    } else if (cmd == "stat") {
      std::vector<std::string> v;
      std::string temp;
      while (line_stream >> temp) {
        v.push_back(temp);
      }
      //	  std::cout<<v.size()<<"\n";
      if (v.size() == 1) {
        std::cout << "Stat Running\n";
        cli.Stat(v[0]);
        continue;
      }
      std::cout << "Stat Error\n";
    } else if (cmd == "delete") {
      std::vector<std::string> v;
      std::string temp;
      while (line_stream >> temp) {
        v.push_back(temp);
      }
      //	  std::cout<<v.size()<<"\n";
      if (v.size() == 1) {
        std::cout << "Delete Running\n";
        cli.Delete(v[0]);
        continue;
      }
      std::cout << "Delete Error\n";
    } else if (cmd == "mkdir") {
      std::vector<std::string> v;
      std::string temp;
      while (line_stream >> temp) {
        v.push_back(temp);
      }
      //	  std::cout<<v.size()<<"\n";
      if (v.size() == 1) {
        std::cout << "Mkdir Running\n";
        cli.Mkdir(v[0]);
        continue;
      }
      std::cout << "Mkdir Error\n";
    } else if (cmd == "list") {
      std::vector<std::string> v;
      std::string temp;
      while (line_stream >> temp) {
        v.push_back(temp);
      }
      //	  std::cout<<v.size()<<"\n";
      if (v.size() == 1) {
        std::cout << "List Running\n";
        cli.List(v[0]);
        continue;
      }
      std::cout << "List Error\n";
    } else if (cmd == "create") {
      std::vector<std::string> v;
      std::string temp;
      while (line_stream >> temp) {
        v.push_back(temp);
      }
      //	  std::cout<<v.size()<<"\n";
      if (v.size() == 1) {
        std::cout << "TestCreate() Running\n";
        cli.TestCreate(v[0]);
        continue;
      }
      std::cout << "TestCreate Error\n";

    } else if (cmd == "write") {
      std::vector<std::string> v;
      std::string temp;
      while (line_stream >> temp) {
        v.push_back(temp);
      }
      //	  std::cout<<v.size()<<"\n";
      if (v.size() == 1) {
        std::cout << "TestWrite() Running\n";
        cli.TestWrite(v[0]);
        continue;
      }
      std::cout << "TestCreate Error\n";
      continue;
    } else if (cmd == "hello") {
      continue;
      // std::vector<std::string> v;
      // std::string temp;
      // while (line_stream >> temp) {
      //   v.push_back(temp);
      // }
      // if (v.size() == 1) {
      //   std::cout << "TestHello() Running\n";
      //   cli.TestHello(v[0]);
      //   continue;
      // }
      // std::cout << "TestHello Error\n";
      // continue;
    } else if (cmd == "findaddr") {
      std::vector<std::string> v;
      std::string temp;
      while (line_stream >> temp) {
        v.push_back(temp);
      }
      if (v.size() == 1) {
        std::cout << "TestFindAddr() Running\n";
        cli.TestFindAddr(v[0]);
        continue;
      }
      std::cout << "TestFindAddr Error\n";
      continue;
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
  return;
}
int main() {
  // InitLogConf("../../common/log.conf");
  RunClient();
  return 0;
}