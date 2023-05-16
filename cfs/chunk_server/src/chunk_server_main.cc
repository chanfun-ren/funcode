#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <string>

#include "chunk_server.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using namespace cfs;

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <port>\n";
    return 1;
  }
  uint32_t port = std::atoi(argv[1]);

  std::shared_ptr<grpc::Channel> channel =
      grpc::CreateChannel("0.0.0.0:5051", grpc::InsecureChannelCredentials());

  ChunkServer chunkserver(channel, "0.0.0.0", port);
  chunkserver.Register();

  chunkserver.StartHeartBeat();
  // TODO:
  chunkserver.UnRegister();
  std::cout << "hello!\n";

  return 0;
}