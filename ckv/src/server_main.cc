#include <iostream>
#include <string>

#include "kv_server.h"

int main(int argc, char* argv[]) {
  int port = 8888;
  std::string db_path = "./db";

  if (argc >= 2) {
    port = std::stoi(argv[1]);
  }

  if (argc >= 3) {
    db_path = argv[2];
  }

  ckv::KvServer kv_server(port, db_path);
  kv_server.Serve();

  return 0;
}