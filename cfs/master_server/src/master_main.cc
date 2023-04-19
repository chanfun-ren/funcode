
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "master_service.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;

void RunServer(const std::string &server_address) {
  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

  cfs::rpc::MasterServiceImpl mastet_service;
  builder.RegisterService(&mastet_service);

  std::unique_ptr<Server> server = builder.BuildAndStart();

  server->Wait();
}

int main() {
  std::string server_address = "0.0.0.0:5051";
  RunServer(server_address);
  return 0;
}