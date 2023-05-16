#include "chunk_server.h"

namespace cfs {

Status ChunkServer::Register() {
  RegisterRequest request;
  ChunkServerLocation* location = request.mutable_chunk_server_location();
  location->set_hostname(hostname_);
  location->set_port(port_);

  RegisterReply reply;
  ClientContext context;

  Status status = stub_->Register(&context, request, &reply);

  if (status.ok()) {
    self_id_ = reply.server_id();
    std::cout << "Successfully registered with server_id: " << reply.server_id()
              << std::endl;
  } else {
    std::cout << "Failed to register: " << status.error_message() << std::endl;
  }
  return status;
}

Status ChunkServer::UnRegister() {
  UnRegisterRequest request;
  request.set_server_id(self_id_);
  ChunkServerLocation* location = request.mutable_chunk_server_location();
  location->set_hostname(hostname_);
  location->set_port(port_);

  UnRegisterReply reply;
  ClientContext context;

  Status status = stub_->UnRegister(&context, request, &reply);
  if (status.ok()) {
    std::cout << "Successfully unregistered" << std::endl;
  } else {
    std::cout << "Failed to unregister: " << status.error_message()
              << std::endl;
  }
  return status;
}

void ChunkServer::StartHeartBeat() {
  while (true) {
    Status status = HeartBeat();
    if (status.ok()) {
      std::cout << "New chunkhandle hearbeat sent " << std::endl;
    }
    std::this_thread::sleep_for(
        std::chrono::seconds(HEARTBEAT_DURATION_SECONDS));
  }
}

Status ChunkServer::HeartBeat() {
  ChunkServerStatisticInfo chunk_server_info;

  // 设置 repeated 字段 chunk_handles
  for (auto chunk_handle : chunk_handles_) {
    chunk_server_info.add_chunk_handles(chunk_handle);
  }

  // 设置 uint64 字段 used_bytes 和 available_bytes
  chunk_server_info.set_used_bytes(used_bytes_);
  chunk_server_info.set_available_bytes(available_bytes_);

  // 设置 ChunkServerLocation 字段 chunk_server_location
  ChunkServerLocation* location =
      chunk_server_info.mutable_chunk_server_location();
  location->set_hostname(hostname_);
  location->set_port(port_);

  HeartBeatRequest request;
  request.set_server_id(self_id_);
  *request.mutable_statistic_info() = chunk_server_info;

  HeartBeatReply reply;
  ClientContext context;

  Status status = stub_->HeartBeat(&context, request, &reply);
  if (status.ok()) {
    // process the HeartBeatReply
  } else {
    std::cout << "Failed to send heartbeat: " << status.error_message()
              << std::endl;
  }
  return status;
}

}  // namespace cfs